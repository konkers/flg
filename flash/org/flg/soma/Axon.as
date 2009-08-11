package org.flg.soma
{
	import org.papervision3d.core.math.*;
	import org.papervision3d.core.proto.*;
	import org.papervision3d.objects.DisplayObject3D;
	import org.papervision3d.objects.primitives.Cylinder;

	public class Axon extends DisplayObject3D
	{
		protected var mat:MaterialObject3D;
		protected var r:Number;
		protected var a:Number;
		protected var n:Number;
		protected var pents:Array;
		protected var c:DisplayObject3D;

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
			c = new DisplayObject3D();

			for (i = 0; i < n + 1; i++) {
				var pos:Number3D;

				pos = new Number3D(0,0,r);

				pents[i] = new Ngon(material, 5, r/15);

				pents[i].rotationY = 90 + i * angle / n;

				pos.rotateY(i * a / n );
				pents[i].position = pos;

				c.addChild(pents[i]);
			}
			c.z = -r;
			addChild(c);
		}
	}
}
