package stuff;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;

public class DataIO {
	private PrintWriter out;
	private BufferedReader buf;
	private String curline;
	private String[] splitted = new String[3];
	int a, b, c;
	int[] ret = new int[3];
	
	public void initWriter(File f) throws FileNotFoundException {
		out = new PrintWriter(f);
	}
	
	public void initReader(File f) throws FileNotFoundException {
		buf = new BufferedReader(new FileReader(f));
	}
	
	
	/**
	 * Setzt die Ausgabedatei und den benötigten Writer und schreibt Metainformationen
	 * @param outputFile			Ausgabedatei
	 * @throws FileNotFoundException
	 */
	public void writeLevelInit(int rows, int columns, int stories) throws IOException {
		out.println(rows+","+columns+","+stories);
	}

	/**
	 * Schreibt ein einzelnes Feld raus
	 * @param type
	 * @param content
	 * @param contentcount
	 */
	public void writeDataLine(int[] data) throws IOException{
		out.println(data[0]+","+data[1]+","+data[2]);
	}
	
	public void closeWriter() {
		out.close();
	}
	
	/**
	 * Bereitet das Einlesen eines Kaufhauses vor
	 * @param inputFile
	 * @throws IOException 
	 */
	public int[] readLevelInit(File inputFile) throws IOException {
		curline = buf.readLine();
		System.out.println(curline);
		String rc[] = curline.split(",");
		ret[0] = Integer.parseInt(rc[0]);
		ret[1] = Integer.parseInt(rc[1]);
		ret[2] = Integer.parseInt(rc[2]);
		return ret;
	}
	
	public int[] readLevelField() throws IOException {
		curline = buf.readLine();
		//System.out.println(curline);
		if(curline == null) {
			int a[] = {42, 42, 42};
			return a;
		}
		splitted = curline.split(",");
		ret[0] = Integer.parseInt(splitted[0]);
		ret[1] = Integer.parseInt(splitted[1]);
		ret[2] = Integer.parseInt(splitted[2]);
		return ret;
	}

	public void closeReader() throws IOException {
		buf.close();
	}
}
