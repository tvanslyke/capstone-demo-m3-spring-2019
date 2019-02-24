#ifndef INO_DIRECTORY_H
#define INO_DIRECTORY_H

#include <Arduino.h>
#include "ProgmemPtr.h"
#include "FlashString.h"
#include "FlashFunction.h"

namespace ino {

struct Directory;
struct DirectoryEntry;

struct DirectoryMethods {
	using get_name_fn    = ino::FlashStringView<> (*)(const DirectoryEntry&);
	using get_parent_fn  = ino::ProgmemPtr<DirectoryEntry> (*)(const DirectoryEntry&);
	using entry_count_fn = std::size_t (*)(const DirectoryEntry&);
	using entry_at_fn    = ino::ProgmemPtr<DirectoryEntry> (*)(const DirectoryEntry&, std::size_t);

	ino::flash_function_type<get_name_fn>    get_name;
	ino::flash_function_type<get_parent_fn>  get_parent;
	ino::flash_function_type<entry_count_fn> entry_count;
	ino::flash_function_type<entry_at_fn>    entry_at;
};

struct DirectoryEntryVTable {
	using display_fn   = int (*)(const DirectoryEntry&, Stream&);
	using set_value_fn = int (*)(const DirectoryEntry&, const char*);

	template <std::size_t N>
	constexpr DirectoryEntryVTable(
		const ino::FlashString<N>& name,
		display_fn display,
		set_value_fn set_value
	):
		display_(display),
		set_value_(set_value),
		type_name_(name)
	{
		
	}

	[[nodiscard]]
	int display(const DirectoryEntry& p, Stream& stream) const {
		auto disp = load_from_flash(display_);
		if(not disp) {
			return command_error(F("Cannot display object."));
		}
		return disp(p, stream);
	}

	[[nodiscard]]
	constexpr ino::FlashStringView<> type_name() const {
		return load_from_flash(type_name_);
	}

	[[nodiscard]]
	constexpr auto set_value(const DirectoryEntry& ent, const char* value) const
		-> ino::FlashStringView<> 
	{
		return load_from_flash(set_value_)(p, value);
	}

	[[nodiscard]]
	constexpr ino::ProgmemPtr<ino::DirectoryMethods> directory_methods() const {
		return load_from_flash(directory_methods_);
	}

	[[nodiscard]]
	constexpr bool is_directory() const {
		return directory_methods();
	}

	[[nodiscard]]
	constexpr ino::FlashStringView<> directory_name(const DirectoryEntry& d) const {
		return directory_methods().get()->get_name(d);
	}

	[[nodiscard]]
	constexpr ino::ProgmemPtr<DirectoryEntry> directory_parent(const DirectoryEntry& d) const {
		return directory_methods().get()->get_parent(d);
	}

	[[nodiscard]]
	constexpr std::size_t directory_entry_count(const DirectoryEntry& d) const {
		return directory_methods().get()->entry_count(d);
	}

	[[nodiscard]]
	constexpr auto directory_entry_at(const DirectoryEntry& d, std::size_t index) const
		-> ino::ProgmemPtr<DirectoryEntry>
	{
		return directory_methods().get()->entry_at(d, index);
	}

private:
	display_fn display_;
	set_value_fn set_value_;
	ino::FlashStringView<> type_name_;
	ino::ProgmemPtr<ino::DirectoryMethods> directory_methods_;
};


struct DirectoryEntry {
	
	template <class T>
	constexpr DirectoryEntry(
		ino::FlashStringView<> name,
		const T* data,
		ino::ProgmemPtr<DirectoryEntry> vtable
	)

	DirectoryEntry from_flash() const {
		return load_from_flash(*this);
	}

	template <class T>
	[[nodiscard]]
	const T* data() const {
		return static_cast<const T*>(this->data_);
	}

	[[nodiscard]]
	ino::FlashStringView<> name() const {
		return name_;
	}

	[[nodiscard]]
	bool is_directory() const {
		return vtable()->is_directory();
	}

	[[nodiscard]]
	int display(Stream& stream) const {
		if(vtable()->display
		vtable()->display(*this, stream);
	}

	constexpr operator bool() const { return data_; }

private:
	const DirectoryEntryVTable* vtable() const {
		return vtable_;
	}
	
	ino::FlashStringView<> name_;
	ino::ProgmemPtr<DirectoryEntryVTable> vtable_;
	const void* data_;
};

template <class T>
[[gnu::progmem]]
inline constexpr auto directory_entry_vtable = DirectoryEntryVTable(
	empty_flash_string, nullptr, nullptr, nullptr
);


struct Directory {
	[[gnu::progmem]]
	static constexpr auto type_name = ino::FlashString{"directory"};

	int display(Stream& stream) const {
		parent().flash_address()->display(stream);
		stream.print('/');
		stream.print(name());
		return 0;
	}

	ino::ProgmemPtr<Directory> parent() const {
		return load_from_flash(parent_);
	}

	ino::FlashStringView<> name() const {
		return load_from_flash(name_);
	}

	ino::ProgmemPtr<DirectoryEntry> entries() const {
		return load_from_flash(entries_);
	}

	std::size_t entry_count() const {
		auto p = entries();
		std::size_t count = 0u;
		while(*p.get()) {
			++p;
			++count;
		}
		return count;
	}

private:
	ino::ProgmemPtr<Directory> parent_;
	ino::FlashStringView<> name_;
	ino::ProgmemPtr<DirectoryEntry> entries_;
};

constexpr DirectoryEntryVTable* directory_vtable() {
	[[gnu::progmem]]
	static constexpr auto directory_methods = DirectoryMethods{
		[](auto ent) {
			return ent.get().data<const Directory*>()->name();
		},
		[](auto ent) {
			return ent.get().data<const Directory*>()->parent();
		},
		[](auto ent) {
			return ent.get().data<const Directory*>()->entry_count();
		},
		[](auto ent, std::size_t index) {
			auto p = ent.get().data<const Directory*>()->entries();
			while(index > 0u) {
				if(not *p.get()) {
					return nullptr;
				}
				++p;
				--index;
			}
			return p;
		}
	};
	[[gnu::progmem]]
	static constexpr auto vtable = DirectoryEntryVTable(
		Directory::type_name,
		[](const DirectoryEntry& ent, Stream& stream) {
			return static_cast<const Directory*>(ent.data())->display(stream);
		},
		nullptr,
		&directory_methods
	};
	return &vtable;
}


} /* namespace ino */

#endif /* INO_DIRECTORY_H */
