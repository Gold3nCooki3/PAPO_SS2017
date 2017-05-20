package window;

import java.awt.Color;
import java.awt.Component;
import java.awt.Container;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Point;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JSlider;
import javax.swing.JTextField;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;


public class WindowManager {
	/**
	 * Fenster und Layout
	 */
	JFrame f = new JFrame("Franc v2.0");
	GridBagLayout gbl = new GridBagLayout();
	Container c = f.getContentPane();

	/**
	 * Das Zeichenfeld
	 */
	DrawPanel dp = new DrawPanel();

	/**
	 * GUI Elemente
	 */
	JButton loadButton = new JButton("Lade Map"); // Lade Map Button
	JButton saveButton = new JButton("Speichere Map"); // Speichere Map Button
	JSlider storySlider = new JSlider(JSlider.VERTICAL, 0, 1337, 0); // Slider
																		// zum
																		// Scrollen
																		// durch
																		// die
																		// Stockwerke
	JLabel storySetterInfo = new JLabel("Anzahl Stockwerke"); // Titel für das
																// Eingabefeld
																// storySetter
	JTextField storySetter = new JTextField("5"); // Eingabefeld für
												// Stockwerkszahl
	JLabel rowSetterInfo = new JLabel("#Zeilen");
	JTextField rowSetter = new JTextField("10");
	JLabel columnsSetterInfo = new JLabel("#Spalten");
	JTextField columnsSetter = new JTextField("10");

	boolean noerrors = true;				//true, wenn alle eingegebene Parameter in den Textfeldern zulässig sind und gespeichert werden darf
	
	public WindowManager() {
		// Init Window
		f.setSize(1000, 800);
		f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		c.setLayout(gbl);
		// Fill Window with first Elements
		// addComponent(c, gbl, c, x, y, width, height, weightx, weighty);
		addComponent(c, gbl, dp, 0, 0, 1, 7, 90, 10);
		addComponent(c, gbl, loadButton, 0, 7, 1, 1, 90, 10);
		addComponent(c, gbl, saveButton, 1, 7, 1, 1, 10, 10);
		addComponent(c, gbl, columnsSetterInfo, 1, 0, 1, 1, 10, 10);
		addComponent(c, gbl, columnsSetter, 1, 1, 1, 1, 10, 10);
		addComponent(c, gbl, rowSetterInfo, 1, 2, 1, 1, 10, 10);
		addComponent(c, gbl, rowSetter, 1, 3, 1, 1, 10, 10);
		addComponent(c, gbl, storySetterInfo, 1, 4, 1, 1, 10, 10);
		addComponent(c, gbl, storySetter, 1, 5, 1, 1, 10, 10);
		addComponent(c, gbl, storySlider, 1, 6, 1, 1, 10, 10);

		// Show Window
		f.setVisible(true);

		addTextfieldListeners();
		addDoubleclickListener();
	}

	/**
	 * Setzt einen Listener auf das DrawPanel dp, damit man Felder mit Doppelklick modifizieren kann
	 */
	private void addDoubleclickListener() {
		//Höre auf Mausklicks und sorge dafür, dass nicht-drag erkannt wird
		dp.addMouseListener(new MouseListener() {
			@Override
			public void mousePressed(MouseEvent me) {
				dp.modifyField(new Point(me.getX(), me.getY()));
				//Deaktiviere Größensetzung rowSetter und columnsSetter
				rowSetter.setEnabled(false);
				columnsSetter.setEnabled(false);
			}
			
			//Öffne Editor
			@Override
			public void mouseReleased(MouseEvent me) {
				dp.invokeEditor();
			}
			
			@Override
			public void mouseExited(MouseEvent arg0) {/*Do nothing*/}
			
			@Override
			public void mouseEntered(MouseEvent arg0) {/*Do nothing*/}
			
			@Override
			public void mouseClicked(MouseEvent arg0) {/*Do nothing*/}
		});
		
		//Höre auf Mausbewegung um Felder zeichnen zu können
		dp.addMouseMotionListener(new MouseMotionListener() {
			
			@Override
			public void mouseMoved(MouseEvent e) {/*Do nothing*/}
			
			@Override
			public void mouseDragged(MouseEvent e) {
				dp.colorFieldsWhenDragged(e.getPoint());
			}
		});
	}


	/**
	 * Prüft, ob der inhalt für ein gegebenes Textfeld aus positiven Zahlen größer als 0 besteht
	 * @param tf
	 */
	private boolean inputchecker(JTextField tf) {
		if (tf.getText().length() > 0) {
			try {
				if (Integer.parseInt(tf.getText()) <= 0) {
					tf.setForeground(Color.red);
					noerrors = false;
					return false;
				} else {
					tf.setForeground(Color.black);
					noerrors = true;
					return true;
				}
			} catch (NumberFormatException e) {
				tf.setForeground(Color.red);
				noerrors = false;
				return false;
			}
		} else {
			tf.setForeground(Color.black);
			noerrors = false;
			return false;
		}
	}

	void addComponent(Container cont, GridBagLayout gbl, Component c, int x, int y, int width, int height,
			double weightx, double weighty) {
		GridBagConstraints gbc = new GridBagConstraints();
		gbc.fill = GridBagConstraints.BOTH;
		gbc.gridx = x;
		gbc.gridy = y;
		gbc.gridwidth = width;
		gbc.gridheight = height;
		gbc.weightx = weightx;
		gbc.weighty = weighty;
		gbl.setConstraints(c, gbc);
		cont.add(c);
	}
	
	private void addTextfieldListeners() {
		// Listener für Änderungen der Spaltenzahl
		columnsSetter.getDocument().addDocumentListener(new DocumentListener() {

			@Override
			public void removeUpdate(DocumentEvent e) {
				// Prüfe ob Zahl>0 und aktualisiere Spaltenzahl, wenn dem so ist
				if (inputchecker(columnsSetter))
					dp.setColumns(Integer.parseInt(columnsSetter.getText()));
			}

			@Override
			public void insertUpdate(DocumentEvent e) {
				// Prüfe ob Zahl>0 und aktualisiere Spaltenzahl, wenn dem so ist
				if (inputchecker(columnsSetter))
					dp.setColumns(Integer.parseInt(columnsSetter.getText()));
			}

			@Override
			public void changedUpdate(DocumentEvent e) {
				// Prüfe ob Zahl>0 und aktualisiere Spaltenzahl, wenn dem so ist
				if (inputchecker(columnsSetter))
					dp.setColumns(Integer.parseInt(columnsSetter.getText()));
			}
		});

		// Listener für Änderungen der Zeilenzahl
		rowSetter.getDocument().addDocumentListener(new DocumentListener() {

			@Override
			public void removeUpdate(DocumentEvent e) {
				// Prüfe ob Zahl>0 und aktualisiere Zeilenzahl, wenn dem so ist
				if (inputchecker(rowSetter))
					dp.setRows(Integer.parseInt(rowSetter.getText()));
			}

			@Override
			public void insertUpdate(DocumentEvent e) {
				// Prüfe ob Zahl>0 und aktualisiere Zeilenzahl, wenn dem so ist
				if (inputchecker(rowSetter))
					dp.setRows(Integer.parseInt(rowSetter.getText()));
			}

			@Override
			public void changedUpdate(DocumentEvent e) {
				// Prüfe ob Zahl>0 und aktualisiere Zeilenzahl, wenn dem so ist
				if (inputchecker(rowSetter))
					dp.setRows(Integer.parseInt(rowSetter.getText()));
			}
		});

		// Listener für Änderungen der Stockwerkszahl
		storySetter.getDocument().addDocumentListener(new DocumentListener() {

			@Override
			public void removeUpdate(DocumentEvent e) {
				// Prüfe ob Zahl>0 und aktualisiere Stockwerkszahl, wenn dem so
				// ist
				if (inputchecker(storySetter))
					dp.setStories(Integer.parseInt(storySetter.getText()));
			}

			@Override
			public void insertUpdate(DocumentEvent e) {
				// Prüfe ob Zahl>0 und aktualisiere Stockwerkszahl, wenn dem so
				// ist
				if (inputchecker(storySetter))
					dp.setStories(Integer.parseInt(storySetter.getText()));
			}

			@Override
			public void changedUpdate(DocumentEvent e) {
				// Prüfe ob Zahl>0 und aktualisiere Stockwerkszahl, wenn dem so
				// ist
				if (inputchecker(storySetter))
					dp.setStories(Integer.parseInt(storySetter.getText()));
			}
		});
	}
}
