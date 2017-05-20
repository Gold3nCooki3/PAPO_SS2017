package stuff;

public class StoryContainer {
	private int rows;
	private int columns;

	private Field[][] storyContent;

	public StoryContainer(int rows, int columns) {
		this.rows = rows;
		this.columns = columns;

		storyContent = new Field[rows][columns];

		init();
	}

	public void addData(int row, int column, int type, int id) {
		// Hier kommt die Unterscheidung auf Gang, Schrank, Rolltreppe, Kassen
		// ... rein und dementsprechend wird das Feld bef�llt
	}

	/**
	 * Setze Feld auf Status "wird aktuell bearbeitet"
	 * 
	 * @param row
	 * @param column
	 */
	public void setWorking(int row, int column) {
		storyContent[row][column].setWorking(true);
	}

	/**
	 * Setter f�r die Zeilen und Reinitialisiuerung
	 * @param rows
	 */
	public void setRows(int rows) {
		this.rows = rows;
		init();
	}
	
	/**
	 * Setter f�r die Spalten und Reinitialisierung
	 * @param columns
	 */
	public void setColumns(int columns) {
		this.columns = columns;
		init();
	}

	/**
	 * Setze Feld auf Status "wird aktuell nicht bearbeitet"
	 * 
	 * @param row
	 * @param column
	 */
	public void setNotWorking(int row, int column) {
		storyContent[row][column].setWorking(false);
	}

	public boolean getWorking(int row, int column) {
		return storyContent[row][column].getWorking();
	}

	/**
	 * Setze Inhalt eines Feldes
	 * @param row
	 * @param column
	 * @param type
	 * @param content
	 */
	public void setData(int row, int column, int type, int content, int contentcount) {
		storyContent[row][column].setData(type, content, contentcount);
	}
	
	/**
	 * Erhalte Inhalt eines Feldes
	 * @param row
	 * @param column
	 * @return
	 */
	public int[] getData(int row, int column) {
		return storyContent[row][column].getData();
	}
	
	private void init() {
		storyContent = new Field[rows][columns];
		// Initialisiere Matrix
		for (int r = 0; r < rows; r++) {
			for (int c = 0; c < columns; c++) {
				storyContent[r][c] = new Field();
			}
		}
	}
}
