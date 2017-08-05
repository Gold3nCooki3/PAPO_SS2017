package window;

import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JSpinner;

import stuff.Coord;
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
				EntityGen eg = new EntityGen(dp.x, dp.y, dp.stories, dp);
				for(int i = 0; i < (int) eCount.getValue(); i++) {
					Coord[] a = eg.generateEntity((int) eMin.getValue(), (int) eMax.getValue());
					for(int u = 0; u < a.length; u++) {
						System.out.println(a[u].x()+" "+a[u].y()+" "+a[u].z());
					}
				}
			}
		});
	}
}