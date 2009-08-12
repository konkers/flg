package org.flg.soma
{
	import org.papervision3d.core.proto.*;
	import org.papervision3d.objects.DisplayObject3D;
	import org.papervision3d.objects.primitives.Cylinder;
	import org.papervision3d.objects.primitives.Sphere;
	import org.papervision3d.objects.primitives.Cone;
	import org.papervision3d.materials.ColorMaterial;
	public class Dendrite extends DisplayObject3D
	{
		protected var c:Array;
		protected var d:Array;
		protected var led:Array;
		protected var m:Array;
		protected var mat:MaterialObject3D;
		protected var l:Number;
		protected var flMat:ColorMaterial;
		protected var fl:Cone;

		public function Dendrite(material:MaterialObject3D,
					 length:Number,
					 structure:Boolean = true)
		{
			var l1:Number;
			var l2:Number;

			mat = material;
			l = length;

			c = new Array(3);
			d = new Array(2);
			led = new Array(2);
			m = new Array(2);

			m[0] = new ColorMaterial(0x00ff00);
			m[1] = new ColorMaterial(0x0000ff);

			c[0] = new Cylinder(mat, length/30, length, 8, 1, -1, false, false);
			c[0].y = length/2;
			if (structure)
				addChild(c[0]);

			l1 = length * 0.8;
			c[1] = new Cylinder(mat, length/30, l1, 8, 1, -1, false, false);
			c[1].y = l1 / 2;

			led[0] = new Sphere(m[0], length/15);
			led[0].y = l1;

			d[0] = new DisplayObject3D();
			if (structure)
				d[0].addChild(c[1]);
			d[0].addChild(led[0]);

			d[0].rotationZ = 30;
			d[0].rotationX = 30;
			d[0].y = length / 3;
			addChild(d[0]);

			l2 = length * 0.4;
			c[2] = new Cylinder(mat, length/30, l2, 8, 1, -1, false, false);
			c[2].y = l2 / 2;

			led[1] = new Sphere(m[1], length/15);
			led[1].y = l2;

			d[1] = new DisplayObject3D();
			if (structure)
				d[1].addChild(c[2]);
			d[1].addChild(led[1]);

			d[1].rotationZ = -30;
			d[1].y = 2 * length / 3;
			addChild(d[1]);

			flMat = new ColorMaterial(0xff8800);
			fl = new Cone(flMat, length/15, l/5, 3, 3);
			fl.y = l + l/10;
			addChild(fl);
		}

		public function setLight(led:Number, color:Number): void
		{
			m[led].fillColor = color;
		}

	}
}
