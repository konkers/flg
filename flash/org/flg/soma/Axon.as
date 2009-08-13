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
				     sections:Number,
				     structure:Boolean = true)
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
				pents[i].visible = structure;
				c.addChild(pents[i]);

				flMat = new ColorMaterial(0xff8800);

				if (i > 0 && i < 11) {
					trace("m[" + (i - 1) + "]");
					m[i-1] = new ColorMaterial(0x00ffff);
					leds[i-1] = new Sphere(m[i-1], 10);
					leds[i-1].rotationY = rot;
					pos = new Number3D(0,0,r*0.9);
					pos.rotateY(i * a / n );
					leds[i-1].position = pos;
					c.addChild(leds[i-1]);
					if (i > 1 && i < 10) {
						var j:Number;
						for (j = 0; j < 3; j++) {
							var f:Number = (i - 2) * 3 + j;

							flames[f] = new Cone(flMat,
									     j == 1 ? 20 : 10,
									     j == 1 ? 120 : 60,
									     3, 3);
							flames[f].rotationX = 90 + ((j - 1) * 20);
							flames[f].rotationY = rot - 90 - angle/(2*n);

							pos = new Number3D(0,0,r*1.05 +
									   (j == 1 ? 60 : 20));


							pos.rotateX((j - 1) * 5);
							pos.rotateY(rot-90-angle/(2*n));
							flames[f].position = pos;
							flames[f].visible = false;
							c.addChild(flames[f]);
						}
					}
				}

			}

			c.z = -r;
			addChild(c);
		}

		public function setLight(led:Number, color:Number):void
		{
			m[9-led].fillColor = color;
		}

		public function setRelay(relay:Number, on:Boolean):void
		{
			flames[23-relay].visible = on;
		}

		public function displayStructure(structure:Boolean = true):void
		{
			var i:Number;

			for (i = 0; i < n + 1; i++) {
				pents[i].visible = structure;
			}
		}
	}
}
