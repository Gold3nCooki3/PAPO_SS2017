package window;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Point;
import java.io.File;
import java.io.IOException;

import javax.swing.JPanel;

import stuff.DataIO;
import stuff.StoryContainer;

@SuppressWarnings("serial")
public class DrawPanel extends JPanel {
	private int rows = 10;		//#horizontale Felder
	private int columns = 10;	//#vertikale Felder
	private float vlinesGap; 
	private float hlinesGap;
	
	int stories = 5;					//#Etagen
	int currentStory = 0;				//Aktuelle Etage
	
	StoryContainer shoppingMall[];
	StoryContainer workingData;								//Aktuelles Stockwerk in Datenform
	
	EditPopup editor = new EditPopup();
	
	/**
	 * 
	 */
	public DrawPanel() {
		setBackground(Color.white);
		initMall();
		setCurrentStory(0);
	}

	/**
	 * �berschreiben der paintComponent
	 */
	@Override
	protected void paintComponent(Graphics g) {
		vlinesGap = (float)this.getWidth() / (float)columns;
		hlinesGap = (float)this.getHeight() /(float) rows;
		super.paintComponent(g);
		drawGrid(g);
		drawWorking(g);
		drawFields(g);
	}
	
	/**
	 * F�rbt die bereits belegten Felder ein 
	 * @param g
	 */
	private void drawFields(Graphics g) {
		// Berechne den Abstand der Linien
		int[] data = new int[2];
		
		for (int r = 0; r < rows; r++) {
			for (int c = 0; c < columns; c++) {
				data = workingData.getData(r, c);
				if(data[0] != 0) {
					switch(data[0]) {
					case 1:					//Gang
						g.setColor(Color.gray);
						break;
					case 2:					//Regal
						g.setColor(Color.blue);
						break;
					case 3:					//Kasse
						g.setColor(Color.green);
						break;
					case 4:					//Aufzug/Rolltreppe
						g.setColor(Color.yellow);
						break;
					default:
						break;
					}
					g.fillRect(Math.round(c*vlinesGap+1),Math.round( r*hlinesGap+1),Math.round(vlinesGap-1),Math.round(hlinesGap-1));
				}
			}
		}
	}

	/**
	 * Zeichne den Hintergrund
	 * @param g
	 */
	private void drawGrid(Graphics g) {
		// Zeichne vertikale Linien
		for (int i = 0; i < columns; i++) {
			g.drawLine(Math.round(i*vlinesGap), 0, Math.round(i *vlinesGap), getHeight());
		}
		// Zeichne horizontale Linien
		for (int i = 0; i < rows; i++) {
			g.drawLine(0, Math.round(i * hlinesGap), getWidth(), Math.round( i * hlinesGap));
		}
	}
	
	/**
	 * Zeichne die aktuell bearbeiteten Felder
	 */
	private void drawWorking(Graphics g) {
		//Aktuell bearbeitete Felder
		for(int r = 0; r < rows; r++) {
			for(int c = 0; c < columns; c++) {
				if(workingData.getWorking(r, c)) {					
					g.setColor(Color.orange);
					g.fillRect(Math.round(c*vlinesGap+1),Math.round( r*hlinesGap+1),Math.round(vlinesGap-1),Math.round(hlinesGap-1));
				}
			}
		}
	}

	/**
	 * Setter f�r Zeilenzahl
	 * 
	 * @param rows
	 */
	public void setRows(int rows) {
		this.rows = rows;
		initMall();
		repaint();
	}

	/**
	 * Setter f�r Spaltenzahl
	 * 
	 * @param columns
	 */
	public void setColumns(int columns) {
		this.columns = columns;
		initMall();
		repaint();
	}
	
	/**
	 * Setter f�r die Anzahl von Stockwerken (stories)
	 * @param story
	 */
	public void setStories(int stories) {
		this.stories = stories;
		shoppingMall = new StoryContainer[stories];
		initMall();
		setCurrentStory(0);
		repaint();
	}
	
	/**
	 * Setze die aktuelle Etage
	 * @param curStory
	 */
	public void setCurrentStory(int curStory) {
		//Speichere erst die aktuelle Etage zur�ck
		shoppingMall[currentStory] = workingData;
		//Wechsle Etage
		this.currentStory = curStory;
		workingData = shoppingMall[this.currentStory];
		repaint();
	}
	
	/**
	 * Getter f�r das aktuelle Stockwerk (currentStory)
	 * @return story
	 */
	public int getCurrentStory() {
		return this.currentStory;
	}

	public void modifyField(Point p) {
		System.out.println("Arbeite mit Position " + p.getX()+", "+p.getY());
		int y = (int) ((p.getX() / this.getWidth()) * columns);
		int x = (int) ((p.getY() / this.getHeight()) * rows);
		try {
			workingData.setWorking(x,  y);
			repaint();
		} catch (ArrayIndexOutOfBoundsException e){
			//Mach einfach nix, interessiert ziemlich wenig
		}
	}

	/**
	 * F�rbe die aktuelle Auswahl zu bearbeitender Felder ein 
	 * @param cur
	 */
	public void colorFieldsWhenDragged(Point cur) {
		int y = (int) ((cur.getX() / this.getWidth()) * columns);
		int x = (int) ((cur.getY() / this.getHeight()) * rows);
		System.out.println(x+" "+y);
		try {
			workingData.setWorking(x,  y);
			repaint();
		} catch (ArrayIndexOutOfBoundsException e){
			//Mach einfach nix, interessiert ziemlich wenig
		}
	}
	
	/**
	 * �ffne den Editor
	 */
	public void invokeEditor() {
		editor.toggleOn(this);
	}
	
	public void saveFromEditor(int type, int content, int contentcount) {
		for (int r = 0; r < rows; r++) {
			for (int c = 0; c < columns; c++) {
				if (workingData.getWorking(r, c)) {
					workingData.setData(r, c, type, content, contentcount);
					workingData.setNotWorking(r, c);
				}
			}
		}
		
		repaint();
	}
	
	/**
	 * Initialisiere Daten
	 */
	private void initMall() {
		shoppingMall = new StoryContainer[stories];
		for (int i = 0; i < stories; i++) {
			shoppingMall[i] = new StoryContainer(rows, columns);
		}
		workingData = shoppingMall[currentStory];
	}

	/**
	 * Starte das Schreiben der Daten
	 * @throws IOException 
	 */
	public void writeData(File saveFile) throws IOException {
		DataIO out = new DataIO();
		out.initWriter(saveFile);
		out.writeLevelInit(rows, columns, stories);
		for(int s = 0; s < stories; s++) {
			for(int r = 0; r < rows; r++) {
				for(int c = 0; c < columns; c++) {
					out.writeDataLine(shoppingMall[s].getData(r, c));
				}
			}
		}
		out.closeWriter();
	}
	
	public void readData(File openFile) throws IOException {
		DataIO in = new DataIO();
		in.initReader(openFile);
		int arr[] = in.readLevelInit(openFile);
		setRows(arr[0]);
		setColumns(arr[1]);
		setStories(arr[2]);
		setCurrentStory(1);
		int buf[] = new int[3];
		for(int s = 0; s < this.stories; s++) {
			workingData = shoppingMall[s];
			for(int r = 0; r < this.rows; r++) {
				for(int c = 0; c < this.columns; c++) {
					buf = in.readLevelField();
					workingData.setData(r, c, buf[0], buf[1], buf[2]);
				}
			}
			shoppingMall[s] = workingData;
		}
		in.closeReader();
		repaint();
	}
}