package window;

import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;

public class EditPopup {
	private JFrame f = new JFrame("Editor");
	private JButton save = new JButton("OK");
	private GridLayout gl = new GridLayout(3, 2, 5, 5);
	
	private String type[] = {"Gang", "Regal", "Rolltreppe", "Aufzug", "Kasse"};
	private String article[] = {"Lebensmittel", "Hygene", "Drogerie", "Kleidung", "Elektro", "Boote", "Sport", "Outdoor", "Parf�m", "Zeugs"};
	
	private JComboBox<String> typeSelector = new JComboBox<>(type);	
	private JComboBox<String> articleSelector = new JComboBox<>(article);
	
	private DrawPanel backhandle;
	
	public EditPopup() {
		f.setSize(300, 400);
		f.setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);
		f.setLayout(gl);
		f.add(new JLabel("Feldtyp ausw�hlen: "));
		f.add(typeSelector);
		f.add(new JLabel("Regalinhalt ausw�hlen: "));
		f.add(articleSelector);
		f.add(new JLabel(""));
		f.add(save);
		save.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e) {
				setContents();
				toggleOff();				
			}
		});
	}
	
	/**
	 * Setze die Parameter und sende sie zur�ck ans DrawPanel
	 */
	protected void setContents() {
		int t = 0, c = 0;
		
		switch(typeSelector.getSelectedItem().toString()) {
		case "Gang":
			t = 1;
			break;
		case "Regal":
			t = 2;
			break;
		case "Kasse":
			t = 3;
			break;
		case "Rolltreppe":
			t = 4;
			break;
		case "Aufzug":
			t = 4;
			break;
		}
		c = 0;
		backhandle.saveFromEditor(t, c, 0);
	}

	public void toggleOn(DrawPanel drawPanel) {
		this.backhandle = drawPanel;
		f.setVisible(true);
	}
	
	private void toggleOff() {
		f.setVisible(false);
	}
}
