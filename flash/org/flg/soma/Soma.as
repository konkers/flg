package org.flg.soma
{
	import flash.events.Event;


	import org.papervision3d.materials.shadematerials.GouraudMaterial;
	import org.papervision3d.materials.shadematerials.FlatShadeMaterial;
	import org.papervision3d.lights.PointLight3D;
	import org.papervision3d.objects.DisplayObject3D;
	import org.papervision3d.objects.primitives.Sphere;
	import org.papervision3d.view.BasicView;

	public class Soma extends BasicView
	{
		protected var sphere:Sphere;
		protected var upperDodeca:Dodeca;
		protected var lowerDodeca:Dodeca;
		protected var axon:Axon;
		protected var soma:DisplayObject3D;

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
			var mat:FlatShadeMaterial;

			light = new PointLight3D();
			light.x = 1000;
			light.y = 1000;
			light.z = -1000;

			soma = new DisplayObject3D();

			mat = new FlatShadeMaterial(light, 0xff0000, 0);

			upperDodeca = new Dodeca(mat, 100, 15);
			upperDodeca.x = -400;
			upperDodeca.y = 350;
			upperDodeca.pitch(-90);

			lowerDodeca = new Dodeca(mat, 100);
			lowerDodeca.x = 400;
			lowerDodeca.y = 50;
			lowerDodeca.pitch(-90);


			axon = new Axon(mat, 500, 90, 10);
			axon.x = -300;
			axon.y = 400;
			axon.roll(30);
			axon.pitch(-90);

			soma.y = -350;

			soma.addChild(upperDodeca);
			soma.addChild(lowerDodeca);
			soma.addChild(axon);
			scene.addChild(soma);
		}

		override protected function onRenderTick(event:Event=null):void
		{
			soma.yaw(1);
			super.onRenderTick(event);
		}
	}
}
