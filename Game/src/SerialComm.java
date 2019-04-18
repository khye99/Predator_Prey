
import jssc.*;

public class SerialComm {

	static SerialPort port;

	private static boolean debug;  // Indicator of "debugging mode"
	
	// This function can be called to enable or disable "debugging mode"
	void setDebug(boolean mode) {
		debug = mode;
	}	
	

	// Constructor for the SerialComm class
	public SerialComm(String name) throws SerialPortException {
		port = new SerialPort(name);		
		port.openPort();
		port.setParams(SerialPort.BAUDRATE_9600,
			SerialPort.DATABITS_8,
			SerialPort.STOPBITS_1,
			SerialPort.PARITY_NONE);
		
		debug = false; // Default is to NOT be in debug mode
	}
		
	// Add writeByte() method from Studio 5
	static void writeByte(byte b) throws SerialPortException {
		port.writeByte(b);
		if (debug == true) {
			try {
				System.out.println(port.readHexString(b));
			} catch (SerialPortException e) {
				e.printStackTrace();
			}
		}
	}
	// Add available() method
	public boolean available() throws SerialPortException {
		return port.getInputBufferBytesCount() > 0;
	}
	// Add readByte() method	
	public byte readByte() throws SerialPortException {
		byte[] foo= port.readBytes(0);
		return foo[0];
	}
	// Add a main() method
	public static void main(String[] args) throws SerialPortException, InterruptedException {
		SerialComm port = new SerialComm("/dev/cu.usbserial-DN03EPK9");
		while(true) {
			if (StdDraw.isKeyPressed(38)) { // up
			SerialComm.writeByte((byte) 'w');
				Thread.sleep(300);
				System.out.println("up");
			}
			else if (StdDraw.isKeyPressed(40)) { // down
			SerialComm.writeByte((byte) 's');
				Thread.sleep(300);
				System.out.println("down");
			}
			else if (StdDraw.isKeyPressed(39)) { // right
				SerialComm.writeByte((byte) 'd');
				Thread.sleep(300);
				System.out.println("right");
			}
			else if (StdDraw.isKeyPressed(37)) { // left
				SerialComm.writeByte((byte) 'a');
				Thread.sleep(300);
				System.out.println("left");
			}
			
		}
		
	}
	
}
