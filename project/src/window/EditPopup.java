package window;

import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JSpinner;

public class EditPopup {
	private JFrame f = new JFrame("Editor");
	private JButton save = new JButton("OK");
	private GridLayout gl = new GridLayout(4, 2, 5, 5);

	private String type[] = { "Gang", "Regal", "Rolltreppe", "Aufzug", "Kasse", "Mauer", "Lager", "Eingang/Ausgang" };
	private String article[] = { "Lebensmittel", "Hygene", "Drogerie", "Kleidung", "Elektro", "Boote", "Sport",
			"Outdoor", "Parf�m", "Zeugs" };

	private JLabel typelabel = new JLabel("Feldtyp auswählen: ");
	private JLabel articlelabel = new JLabel("Regalinhalt auswählen: ");
	private JLabel countlabel = new JLabel("Stückzahl: ");

	private JComboBox<String> typeSelector = new JComboBox<>(type);
	private JComboBox<String> articleSelector = new JComboBox<>(article);
	private JSpinner count = new JSpinner();

	private DrawPanel backhandle;

	public EditPopup() {
		f.setSize(300, 400);
		f.setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);
		f.setLayout(gl);
		f.add(typelabel);
		f.add(typeSelector);
		f.add(articlelabel);
		f.add(articleSelector);
		f.add(countlabel);
		f.add(count);
		f.add(new JLabel(""));
		f.add(save);

		// Behandelt einblenden/ausblenden von Felder, je nach Feldtyp
		typeSelector.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {
				switch (typeSelector.getSelectedItem().toString()) {
				case "Lager":
				case "Regal":
					countlabel.setEnabled(true);
					count.setEnabled(true);
					articlelabel.setEnabled(true);
					articleSelector.setEnabled(true);
					break;
				default:
					count.setEnabled(false);
					countlabel.setEnabled(false);
					articlelabel.setEnabled(false);
					articleSelector.setEnabled(false);

				}
			}
		});

		// Behandelt Button Event
		save.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {
				setContents();
				toggleOff();
			}
		});
	}

	/**
	 * Setze die Parameter und sende sie zurück ans DrawPanel
	 */
	protected void setContents() {
		int t = 0, c = 0, cc = 0;

		switch (typeSelector.getSelectedItem().toString()) {
		case "Gang":
		case "Kasse":
		case "Rolltreppe":
		case "Aufzug":
		case "Mauer":
		case "Eingang/Ausgang":
			t = typeSelector.getSelectedIndex();
			break;
		case "Regal":
		case "Lager":
			t = typeSelector.getSelectedIndex();
			c = articleSelector.getSelectedIndex();
			// Geht, weil Standard-Model einen Integer bereitstellt
			cc = (int) count.getValue();
			break;
		default:
			System.out.println("wtf?");
		}
		System.out.println("Habe typ "+t+" gesetzt");
		backhandle.saveFromEditor(t, c, cc);
	}

	public void toggleOn(DrawPanel drawPanel) {
		this.backhandle = drawPanel;
		f.setVisible(true);
	}

	private void toggleOff() {
		f.setVisible(false);
	}

	public void getCurrentValues(int[] data) {
		typeSelector.setSelectedIndex(data[0]);
		articleSelector.setSelectedIndex(data[1]);
		count.setValue(data[2]);
	}
}
