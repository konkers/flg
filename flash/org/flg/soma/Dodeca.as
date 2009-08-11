package org.flg.soma
{
	import org.papervision3d.core.math.*;
	import org.papervision3d.core.proto.*;
	import org.papervision3d.objects.DisplayObject3D;
	import org.papervision3d.objects.primitives.Cylinder;

	public class Dodeca extends DisplayObject3D
	{
		protected var mat:MaterialObject3D;
		protected var r:Number;
		protected var pents:Array;
		protected var dends:Array;

		private static const normals:Array = [
			new Number3D( 0.000000,  0.000000,  1.000000),
			new Number3D( 0.894427,  0.000000,  0.447214),
			new Number3D( 0.276393, -0.850651,  0.447214),
			new Number3D(-0.723607, -0.525731,  0.447214),
			new Number3D(-0.723607,  0.525731,  0.447214),
			new Number3D( 0.276393,  0.850651,  0.447214),
			new Number3D(-0.894427,  0.000000, -0.447214),
			new Number3D(-0.276393,  0.850651, -0.447214),
			new Number3D( 0.723607,  0.525731, -0.447214),
			new Number3D( 0.723607, -0.525731, -0.447214),
			new Number3D(-0.276393, -0.850651, -0.447214),
			new Number3D( 0.000000, 0.0000000, -1.000000),
			];

		private static const phi:Number = 1.6180339887;
		private static const invPhi:Number = 0.6180339887;


		private static const verts:Array = [
			new Number3D(-1, -1, -1),
			new Number3D(-1, -1,  1),
			new Number3D(-1,  1, -1),
			new Number3D(-1,  1,  1),
			new Number3D( 1, -1, -1),
			new Number3D( 1, -1,  1),
			new Number3D( 1,  1, -1),
			new Number3D( 1,  1,  1),

			new Number3D(0,  invPhi,  phi),
			new Number3D(0,  invPhi, -phi),
			new Number3D(0, -invPhi,  phi),
			new Number3D(0, -invPhi, -phi),

			new Number3D( invPhi,  phi, 0),
			new Number3D( invPhi, -phi, 0),
			new Number3D(-invPhi,  phi, 0),
			new Number3D(-invPhi, -phi, 0),

			new Number3D( phi, 0,  invPhi),
			new Number3D( phi, 0, -invPhi),
			new Number3D(-phi, 0,  invPhi),
			new Number3D(-phi, 0, -invPhi),
			];

		private function dist(x1:Number, y1:Number, z1:Number,
				      x2:Number, y2:Number, z2:Number):Number
		{
			return Math.sqrt((x2 - x1) * (x2 - x1)  +
					 (y2 - y1) * (y2 - y1)  +
					 (z2 - z1) * (z2 - z1));
		}

		public function Dodeca(material:MaterialObject3D, radius:Number, dendrites:Number = 5)
		{
			var i:Number;
			var penR:Number;
			var xAxis:Number3D = new Number3D(1,0,0);
			var yAxis:Number3D = new Number3D(0,1,0);
			var zAxis:Number3D = new Number3D(0,0,1);
			var yRot:Number;
			var zRot:Number;

			mat = material;
			r = radius;

			pents = new Array(12);

			for (i = 0; i < normals.length ; i++) {
				var x:Number = normals[i].x;
				var y:Number = normals[i].y;
				var z:Number = normals[i].z;

				pents[i] = new Ngon(material, 5, r/1.5);


				if (x != 0 || z != 0) {
					yRot = Math.atan(Math.sqrt(x*x + y*y)/z) *
						180 / Math.PI;
					zRot = Math.atan(y/x) *
						180 / Math.PI;
					if (x < 0) {
						yRot += 180;
						yRot *= -1;
					}
					trace( "y = " + yRot + " z = " + zRot );
					pents[i].rotationY = yRot;
					pents[i].rotationZ = zRot;
				} else if (z > 0.9) {
					pents[i].rotationZ = 180;
				}

				pents[i].x = normals[i].x * r;
				pents[i].y = normals[i].y * r;
				pents[i].z = normals[i].z * r;

				trace("foo" + pents[i].x + " " + pents[i].y+ " " + pents[i].z + " " );

				addChild(pents[i]);
			}

			dends = new Array(verts.length);

			for (i = 0; i < verts.length ; i++) {
				var v:Number3D = verts[i].clone();
				v.normalize();
				v.rotateY(30);
				x = v.x;
				y = v.y;
				z = v.z;

				dends[i] = new Dendrite(mat, r*2);
				if (x != 0 && z != 0) {
					yRot = Math.atan(Math.sqrt(x*x + y*y)/z) *
						180 / Math.PI;
					zRot = Math.atan(y/x) *
						180 / Math.PI;
					if (x < 0) {
						yRot *= -1;
					}
					if (z < 0) {
						yRot += 180;
					}

					if ((dendrites == 5 && z > 0.5) ||
					     (dendrites == 15 && z > -0.5)){
						dends[i].rotationX = yRot + 90;
						dends[i].rotationZ = zRot + 90 ;
						dends[i].x = x * (r) *1.1;
						dends[i].y = y * (r) *1.1;
						dends[i].z = z * (r) *1.1;
						addChild(dends[i]);
					}
				}
			}
		}
	}
}
