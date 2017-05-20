package window;

import java.awt.Color;
import java.awt.Component;
import java.awt.Container;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.io.IOException;

import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JSlider;
import javax.swing.JTextField;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
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
	JLabel storySetterInfo = new JLabel("Anzahl Stockwerke"); // Titel f�r das
																// Eingabefeld
																// storySetter
	JTextField storySetter = new JTextField("5"); // Eingabefeld f�r
												// Stockwerkszahl
	JLabel curStoryInfo = new JLabel("Aktuelles Stockwerk: " + 1);
	JLabel rowSetterInfo = new JLabel("#Zeilen");
	JTextField rowSetter = new JTextField("10");
	JLabel columnsSetterInfo = new JLabel("#Spalten");
	JTextField columnsSetter = new JTextField("10");

	boolean noerrors = true;				//true, wenn alle eingegebene Parameter in den Textfeldern zul�ssig sind und gespeichert werden darf
	
	public WindowManager() {
		// Init Window
		f.setSize(1000, 800);
		f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		c.setLayout(gbl);
		storySlider.setMaximum(Integer.parseInt(storySetter.getText()));
		storySlider.setMinimum(1);
		storySlider.setMajorTickSpacing(Integer.parseInt(storySetter.getText()) / 5);
		storySlider.setPaintLabels(true);
		// Fill Window with first Elements
		// addComponent(c, gbl, c, x, y, width, height, weightx, weighty);
		addComponent(c, gbl, dp, 0, 0, 1, 8, 90, 10);
		addComponent(c, gbl, loadButton, 0, 8, 1, 1, 90, 10);
		addComponent(c, gbl, saveButton, 1, 8, 1, 1, 10, 10);
		addComponent(c, gbl, columnsSetterInfo, 1, 0, 1, 1, 10, 10);
		addComponent(c, gbl, columnsSetter, 1, 1, 1, 1, 10, 10);
		addComponent(c, gbl, rowSetterInfo, 1, 2, 1, 1, 10, 10);
		addComponent(c, gbl, rowSetter, 1, 3, 1, 1, 10, 10);
		addComponent(c, gbl, storySetterInfo, 1, 4, 1, 1, 10, 10);
		addComponent(c, gbl, storySetter, 1, 5, 1, 1, 10, 10);
		addComponent(c, gbl, curStoryInfo, 1, 6, 1, 1, 10, 10);
		addComponent(c, gbl, storySlider, 1, 7, 1, 1, 10, 10);

		// Show Window
		f.setVisible(true);

		addTextfieldListeners();
		addClickListener();
		addButtonListeners();
		addSliderListener();
	}

	/**
	 * Überwacht die Buttons auf Klickevents
	 */
	private void addButtonListeners() {
		//Ladebutton
		loadButton.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e) {
				JFileChooser auswahl = new JFileChooser();
				int action = auswahl.showOpenDialog(null);
				if(action == JFileChooser.APPROVE_OPTION) {
					try {
						dp.readData(auswahl.getSelectedFile());
					} catch (IOException e1) {
						// TODO Auto-generated catch block
						e1.printStackTrace();
					}
				}
			}
		});
		
		
		//Speicherbutton
		saveButton.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e) {
				//Schreibe Kaufhaus in Datei
				//1. Starte Ausgabedialog
				//2. Schreibe
				JFileChooser auswahl = new JFileChooser();
				int action = auswahl.showSaveDialog(null);
				if(action == JFileChooser.APPROVE_OPTION) {
					try {
						dp.writeData(auswahl.getSelectedFile());
					} catch (IOException ioe) {
						System.err.println(ioe);
					}
				}
			}
		});
	}

	/**
	 * Setzt einen Listener auf das DrawPanel dp, damit man Felder modifizieren kann
	 */
	private void addClickListener() {
		//H�re auf Mausklicks und sorge daf�r, dass nicht-drag erkannt wird
		dp.addMouseListener(new MouseListener() {
			@Override
			public void mousePressed(MouseEvent me) {
				dp.modifyField(new Point(me.getX(), me.getY()));
				//Deaktiviere Gr��ensetzung rowSetter und columnsSetter
				rowSetter.setEnabled(false);
				columnsSetter.setEnabled(false);
			}
			
			//�ffne Editor
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
		
		//H�re auf Mausbewegung um Felder zeichnen zu k�nnen
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
	 * Pr�ft, ob der inhalt f�r ein gegebenes Textfeld aus positiven Zahlen gr��er als 0 besteht
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
		// Listener f�r �nderungen der Spaltenzahl
		columnsSetter.getDocument().addDocumentListener(new DocumentListener() {

			@Override
			public void removeUpdate(DocumentEvent e) {
				// Pr�fe ob Zahl>0 und aktualisiere Spaltenzahl, wenn dem so ist
				if (inputchecker(columnsSetter))
					dp.setColumns(Integer.parseInt(columnsSetter.getText()));
			}

			@Override
			public void insertUpdate(DocumentEvent e) {
				// Pr�fe ob Zahl>0 und aktualisiere Spaltenzahl, wenn dem so ist
				if (inputchecker(columnsSetter))
					dp.setColumns(Integer.parseInt(columnsSetter.getText()));
			}

			@Override
			public void changedUpdate(DocumentEvent e) {
				// Pr�fe ob Zahl>0 und aktualisiere Spaltenzahl, wenn dem so ist
				if (inputchecker(columnsSetter))
					dp.setColumns(Integer.parseInt(columnsSetter.getText()));
			}
		});

		// Listener f�r �nderungen der Zeilenzahl
		rowSetter.getDocument().addDocumentListener(new DocumentListener() {

			@Override
			public void removeUpdate(DocumentEvent e) {
				// Pr�fe ob Zahl>0 und aktualisiere Zeilenzahl, wenn dem so ist
				if (inputchecker(rowSetter))
					dp.setRows(Integer.parseInt(rowSetter.getText()));
			}

			@Override
			public void insertUpdate(DocumentEvent e) {
				// Pr�fe ob Zahl>0 und aktualisiere Zeilenzahl, wenn dem so ist
				if (inputchecker(rowSetter))
					dp.setRows(Integer.parseInt(rowSetter.getText()));
			}

			@Override
			public void changedUpdate(DocumentEvent e) {
				// Pr�fe ob Zahl>0 und aktualisiere Zeilenzahl, wenn dem so ist
				if (inputchecker(rowSetter))
					dp.setRows(Integer.parseInt(rowSetter.getText()));
			}
		});

		// Listener f�r �nderungen der Stockwerkszahl
		storySetter.getDocument().addDocumentListener(new DocumentListener() {

			@Override
			public void removeUpdate(DocumentEvent e) {
				// Pr�fe ob Zahl>0 und aktualisiere Stockwerkszahl, wenn dem so
				// ist
				if (inputchecker(storySetter)) {
					int zahl = Integer.parseInt(storySetter.getText());
					dp.setStories(zahl);
					storySlider.setMaximum(zahl);
					storySlider.setMajorTickSpacing((int) zahl/5);
					storySlider.setPaintLabels(true);
				}
			}

			@Override
			public void insertUpdate(DocumentEvent e) {
				// Pr�fe ob Zahl>0 und aktualisiere Stockwerkszahl, wenn dem so
				// ist
				if (inputchecker(storySetter)) {
					int zahl = Integer.parseInt(storySetter.getText());
					dp.setStories(zahl);
					storySlider.setMaximum(zahl);
					storySlider.setMajorTickSpacing((int) zahl/5);
					storySlider.setPaintLabels(true);
				}
			}

			@Override
			public void changedUpdate(DocumentEvent e) {
				// Pr�fe ob Zahl>0 und aktualisiere Stockwerkszahl, wenn dem so
				// ist
				if (inputchecker(storySetter)) {
					int zahl = Integer.parseInt(storySetter.getText());
					dp.setStories(zahl);
					storySlider.setMaximum(zahl);
					storySlider.setMajorTickSpacing((int) zahl/5);
					storySlider.setPaintLabels(true);
				}
			}
		});
	}
	
	private void addSliderListener() {
		storySlider.addChangeListener(new ChangeListener() {
			
			@Override
			public void stateChanged(ChangeEvent e) {
				dp.setCurrentStory(storySlider.getValue() - 1);
				curStoryInfo.setText("Aktuelles Stockwerk: " + storySlider.getValue());
			}
		});
	}
}
