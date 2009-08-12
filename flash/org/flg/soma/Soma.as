package org.flg.soma
{
	import flash.events.*;
	import flash.utils.*;

	import flash.net.URLLoader;
	import flash.net.URLRequest;

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
		protected var timer:Timer;

		protected const displayStructure:Boolean = false;

		protected const nAxonLights:Number = 10;
		protected const nAxonLightOffset:Number = 0;

		protected const nUDLights:Number = 30;
		protected const nUDLightOffset:Number = nAxonLightOffset + nAxonLights;

		protected const nLDLights:Number = 10;
		protected const nLDLightOffset:Number = nUDLightOffset + nUDLights;

		protected var state:Array;

		public function Soma()
		{
			super(1, 1, true, false);


			opaqueBackground = 0;

			createModel();

			state = new Array(nAxonLights + nUDLights + nLDLights);
			getState();
			startRendering();

		}

		private function getState():void
		{
			var loader:URLLoader;
			var request:URLRequest;

			request = new URLRequest("http://127.0.0.1:8080/soma/state");
			loader = new URLLoader();
			loader.addEventListener(Event.COMPLETE, completeHandler);
			loader.addEventListener(Event.OPEN, openHandler);
			loader.addEventListener(ProgressEvent.PROGRESS, progressHandler);
			loader.addEventListener(SecurityErrorEvent.SECURITY_ERROR, securityErrorHandler);
			loader.addEventListener(HTTPStatusEvent.HTTP_STATUS, httpStatusHandler);
			loader.addEventListener(IOErrorEvent.IO_ERROR, ioErrorHandler);

			try {
				loader.load(request);
			} catch (error:Error) {
				trace("Unable to load requested document.");
			}
		}

		private function completeHandler(event:Event):void {
			var loader:URLLoader = URLLoader(event.target);
			var lines:Array = loader.data.split("\n");
			var i:Number;
			var match:Array;
			var index:Number;
			var val:Number;

			for (i = 0; i < lines.length; i++) {
				match = lines[i].split(": ");

				if (match.length == 2) {
					index = parseInt(match[0], 16);
					val = parseInt(match[1], 16);

					state[index] = val;
				}
			}
			getState();
//             var vars:URLVariables = new URLVariables(loader.data);
//             trace("The answer is " + vars.answer);
		}

		private function openHandler(event:Event):void {
//			trace("openHandler: " + event);
		}

		private function progressHandler(event:ProgressEvent):void {
//			trace("progressHandler loaded:" + event.bytesLoaded + " total: " + event.bytesTotal);
		}

		private function securityErrorHandler(event:SecurityErrorEvent):void {
			trace("securityErrorHandler: " + event);
		}

		private function httpStatusHandler(event:HTTPStatusEvent):void {
//			trace("httpStatusHandler: " + event);
		}

		private function ioErrorHandler(event:IOErrorEvent):void {
			trace("ioErrorHandler: " + event);
		}

		protected function createModel():void
		{
			var mat:FlatShadeMaterial;

			light = new PointLight3D();
			light.x = 1000;
			light.y = 1000;
			light.z = -1000;

			soma = new DisplayObject3D();

			mat = new FlatShadeMaterial(light, 0x808080, 0);

			upperDodeca = new Dodeca(mat, 100, 15, displayStructure);
			upperDodeca.x = -400;
			upperDodeca.y = 350;
			upperDodeca.pitch(-90);

			lowerDodeca = new Dodeca(mat, 100, 5, displayStructure);
			lowerDodeca.x = 400;
			lowerDodeca.y = 50;
			lowerDodeca.pitch(-90);


			axon = new Axon(mat, 500, 90, 12, displayStructure);
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
			var i:Number;
			for (i = 0; i < nAxonLights; i++) {
				axon.setLight(i,state[i]);
			}
			for (i = 0; i < nUDLights; i++) {
				upperDodeca.setLight(i,state[i + nUDLightOffset]);
			}
			for (i = 0; i < nLDLights; i++) {
				lowerDodeca.setLight(i,state[i + nLDLightOffset]);
			}

			soma.yaw(1);
			super.onRenderTick(event);
		}
	}
}
