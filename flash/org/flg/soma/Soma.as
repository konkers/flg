package org.flg.soma
{
	import org.papervision3d.materials.shadematerials.GouraudMaterial;
	import org.papervision3d.lights.PointLight3D;
	import org.papervision3d.objects.primitives.Sphere;
	import org.papervision3d.view.BasicView;

	public class Soma extends BasicView
	{
		protected var sphere:Sphere;
		protected var dodeca:Dodeca;

		protected var light:PointLight3D;
		public function Soma()
		{
			super(1, 1, true, false);

			opaqueBackground = 0;

			createModel();

			startRendering();
		}

		protected function createModel():void
		{
			var mat:GouraudMaterial;

			light = new PointLight3D();
			light.x = 1000;
			light.y = 1000;
			light.z = -1000;

			mat = new GouraudMaterial(light, 0xff0000, 0);
			dodeca = new Dodeca(mat, 200);
			scene.addChild(dodeca);
//			sphere = new Sphere(mat, 200, 20, 20);
//			scene.addChild(sphere);

		}
	}
}
