package org.flg.soma
{
	import org.papervision3d.core.math.*;
	import org.papervision3d.core.proto.*;
	import org.papervision3d.objects.DisplayObject3D;
	import org.papervision3d.materials.ColorMaterial;
	import org.papervision3d.objects.primitives.Cylinder;
	import org.papervision3d.objects.primitives.Sphere;
	import org.papervision3d.objects.primitives.Cone;

	public class Axon extends DisplayObject3D
	{
		protected var mat:MaterialObject3D;
		protected var r:Number;
		protected var a:Number;
		protected var n:Number;
		protected var pents:Array;
		protected var c:DisplayObject3D;
		protected var leds:Array;
		protected var m:Array;
		protected var flames:Array;
		protected var flMat:ColorMaterial;


		public function Axon(material:MaterialObject3D,
				     radius:Number,
				     angle:Number,
				     sections:Number)
		{
			var i:Number;

			mat = material;
			r = radius;
			a = angle;
			n = sections;

			pents = new Array(n+1);
			leds = new Array(10);
			m = new Array(10);
			flames = new Array(24);
			c = new DisplayObject3D();

			for (i = 0; i < n + 1; i++) {
				var pos:Number3D;
				var rot:Number;

				pos = new Number3D(0,0,r);

				pents[i] = new Ngon(material, 5, r/15);

				rot = 90 + i * angle / n;
				pents[i].rotationY = rot;

				pos.rotateY(i * a / n );
				pents[i].position = pos;
				c.addChild(pents[i]);

				if (i > 0 && i<12) {
					m[i-1] = new ColorMaterial(0x00ffff);
					leds[i-1] = new Sphere(m[i-1], 10);
					leds[i-1].rotationY = rot;
					pos = new Number3D(0,0,r*0.9);
					pos.rotateY(i * a / n );
					leds[i-1].position = pos;
					c.addChild(leds[i-1]);
				}

			}

			flMat = new ColorMaterial(0xff8800);

			for (i = 0; i < 24; i++) {
				flames[i] = new Cone(flMat, 10, 60, 3, 3);
				flames[i].rotationX = 90;
				flames[i].rotationY = (i + 2) * a / (24 + 4);
				pos = new Number3D(0,0,r*1.1);

				pos.rotateY((i + 2) * a / (24 + 4));
				flames[i].position = pos;
				c.addChild(flames[i]);
			}

			c.z = -r;
			addChild(c);
		}
	}
}
