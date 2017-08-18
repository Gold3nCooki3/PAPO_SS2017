package window;

import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.IOException;

import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JSpinner;

import stuff.Coord;
import stuff.DataIO;
import stuff.EntityGen;

public class EntityGenPopup {
	private GridLayout gl = new GridLayout(4, 2, 5, 5);
	private JFrame f = new JFrame();
	private JLabel elemMin = new JLabel("Mindestanzahl von Elementen auf der Liste");
	private JLabel elemMax = new JLabel("Höchstzahl von Elementen auf der Liste");
	private JLabel entcount = new JLabel("Wie viele Kunden sollen generiert werden?");
	private JSpinner eMin = new JSpinner(); 
	private JSpinner eMax = new JSpinner();
	private JSpinner eCount = new JSpinner();
	private JLabel platzhalter = new JLabel("");
	private JButton gen = new JButton("Crash inbound!");
	DataIO dio = new DataIO();
	private int id = 0;
	
	public EntityGenPopup(DrawPanel dp) {
		f.setSize(700, 200);
		f.setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);
		f.setLayout(gl);
		f.setVisible(true);
		f.add(elemMin);
		f.add(eMin);
		f.add(elemMax);
		f.add(eMax);
		f.add(entcount);
		f.add(eCount);
		f.add(platzhalter);
		f.add(gen);
		
		gen.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e) {
				EntityGen eg = new EntityGen(dp.rows, dp.columns, dp.stories, dp);
				JFileChooser speicher = new JFileChooser();
				int action = speicher.showSaveDialog(null);
				if(action == JFileChooser.APPROVE_OPTION) {
					try {
						if(!speicher.getSelectedFile().getName().endsWith(".elist")) {
							dio.initEntityWriter(new File(speicher.getSelectedFile().toString().concat(".elist")), (int) eMax.getValue(), (int) eCount.getValue());
						} else {
							dio.initEntityWriter(speicher.getSelectedFile(), (int) eMax.getValue(), (int) eCount.getValue());
						}
					} catch (IOException ioe) {
						System.err.println(ioe);
					}
				}
				for(int i = 0; i < (int) eCount.getValue(); i++) {
					Coord[] a = eg.generateEntity((int) eMin.getValue(), (int) eMax.getValue());
					try {
						dio.writeEntity(id++, a, (int) eMax.getValue());
					} catch (IOException e2) {
						e2.printStackTrace();
					}
				}
				try {
					dio.closeEntityWriter();
				} catch (IOException e2) {
					e2.printStackTrace();
				}
				f.dispose();
			}
		});
	}
}