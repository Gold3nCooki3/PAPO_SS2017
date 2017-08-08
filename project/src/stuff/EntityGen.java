package stuff;

import java.util.Random;

import window.DrawPanel;

public class EntityGen {
	private int storyX;
	private int storyY;
	private int storyZ;
	private DrawPanel dp;
	
	public EntityGen(int x, int y, int z, DrawPanel dp) {
		storyX = x;
		storyY = y;
		storyZ = z;
		this.dp = dp;
	}
	
	public Coord[] generateEntity(int minElems, int maxElems) {
		Random r = new Random();
		int elems = r.nextInt(maxElems-minElems+1) + minElems;
		Coord list[] = new Coord[elems];
		Random rx = new Random();
		Random ry = new Random();
		Random rz = new Random();
		
		int tx, ty, tz;
		int i = 0;		
		while(i < elems) {
			tx = rx.nextInt(storyX);
			ty = ry.nextInt(storyY);
			tz = rz.nextInt(storyZ);
			
			if(dp.shoppingMall[tz].getData(tx, ty)[0] == 1) {
				list[i] = new Coord(tx, ty, tz);
				i++;
			}
		}
		return list;
	}
}