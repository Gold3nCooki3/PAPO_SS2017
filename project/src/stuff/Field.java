package stuff;

public class Field {
	/**
	 * 0 == frei
	 * 1 = Gang = grau
	 * 2 = Schrank = Farbe nach Inhalt
	 * 3 = Kasse = gr�n
	 * 4 = Rolltreppe/Aufzug = gelb
	 */
	int type = 0;			//Definiert ob Gang, Schrank, Kasse, Rolltreppe, ...
	int content = 0;		//
	int contentcount = 0;
	boolean working = false;
	
	public Field() {
		working = false;
	}
	
	/**
	 * Setze den aktuellen Bearbeitungsstatus
	 * @param working
	 */
	void setWorking(boolean working) {
		this.working = working;
	}
	
	/**
	 * Liefere den aktuellen Bearbeitungsstatus
	 * @return 
	 */
	boolean getWorking() {
		return working;
	}

	public void setData(int type, int content, int contentcount) {
		this.type = type;
		this.content = content;
		this.contentcount = contentcount;
	}
	
	public int[] getData() {
		int ret[] = {this.type, this.content, this.contentcount}; 
		return ret;
	}
}
