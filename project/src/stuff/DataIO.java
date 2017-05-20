package stuff;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintWriter;

public class DataIO {
	private PrintWriter out;

	/**
	 * Setzt die Ausgabedatei und den benötigten Writer und schreibt Metainformationen
	 * @param outputFile			Ausgabedatei
	 * @throws FileNotFoundException
	 */
	public void writeLevelInit(File outputFile, int rows, int columns) throws IOException {
		out = new PrintWriter(outputFile);
		out.println(rows+"/"+columns);
	}

	/**
	 * Schreibt ein einzelnes Feld raus
	 * @param type
	 * @param content
	 * @param contentcount
	 */
	public void writeDataLine(int type, int content, int contentcount) throws IOException{
		out.println(type+","+content+","+contentcount);
	}
	
	/**
	 * Wird sich noch als unnötig herausstellen
	 * @param story
	 * @throws IOException
	 */
	public void writeStoryChange(int story) throws IOException{
		out.println("+");
	}
	
	/**
	 * Bereitet das einlesen eines Kaufhauses vor
	 * @param inputFile
	 */
	public void readLevelInit(File inputFile) {
		
	}
}
