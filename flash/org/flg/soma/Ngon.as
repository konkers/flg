package org.flg.soma
{
	import org.papervision3d.core.proto.*;
	import org.papervision3d.objects.DisplayObject3D;
	import org.papervision3d.objects.primitives.Cylinder;

	public class Ngon extends DisplayObject3D
	{
		protected var cylinders:Array;
		protected var mat:MaterialObject3D;
		protected var r:Number;
		protected var n:Number;


		public function Ngon(material:MaterialObject3D, sides:Number, radius:Number)
		{
			var i:Number;
			var length:Number;
			var offset:Number;
			var angle:Number;

			super();

			mat = material;
			r = radius;
			n = sides;

			cylinders = new Array(n);

			angle = 2.0 * Math.PI / n;

			length = 2.0 * r * Math.sin(angle / 2.0);
			offset = Math.sqrt(r*r - (length / 2.0) * (length / 2.0));
			for (i = 0; i < n; i++) {
				var a:Number;

				a = angle * i + (angle / 2.0);

				cylinders[i] = new Cylinder(mat, 10, length);
				cylinders[i].x = Math.cos(a) * offset;
				cylinders[i].y = Math.sin(a) * offset;
				cylinders[i].rotationZ = a * 360 / (2 * Math.PI);

				addChild(cylinders[i]);
			}
		}
	}
}
