import serial
import readline
import threading
import sys


with serial.Serial(port='/dev/ttyUSB0', baudrate=115200, dsrdtr=False) as conn:
	def write_output():
		try:
			while conn.isOpen():
				sys.stdout.write(conn.read_all().decode("utf8"))
		except OSError as e:
			sys.stderr.write("Connection closed with error: '{}'\n".format(str(e)))

	thrd = threading.Thread(target=write_output)
	thrd.start()
	while conn.isOpen():
		cmd = input("")
		if cmd == "exit" or cmd == "quit":
			break
		conn.write((cmd + '\n').encode("utf8"))
	

	 



