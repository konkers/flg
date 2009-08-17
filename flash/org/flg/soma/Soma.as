package org.flg.soma
{
	import flash.events.*;
	import flash.utils.*;
	import flash.display.*;

	import flash.net.URLLoader;
	import flash.net.URLRequest;

	import org.papervision3d.materials.shadematerials.GouraudMaterial;
	import org.papervision3d.materials.shadematerials.FlatShadeMaterial;
	import org.papervision3d.materials.special.Letter3DMaterial;
	import org.papervision3d.lights.PointLight3D;
	import org.papervision3d.objects.DisplayObject3D;
	import org.papervision3d.objects.primitives.Sphere;
	import org.papervision3d.view.BasicView;
	import org.papervision3d.typography.*;
	import org.papervision3d.typography.fonts.*;


	public class Soma extends BasicView
	{
		protected var sphere:Sphere;
		protected var upperDodeca:Dodeca;
		protected var lowerDodeca:Dodeca;
		protected var axon:Axon;
		protected var soma:DisplayObject3D;
		protected var light:PointLight3D;
		protected var timer:Timer;


		protected const nAxonLights:Number = 10;
		protected const nAxonLightOffset:Number = 0;

		protected const nUDLights:Number = 30;
		protected const nUDLightOffset:Number = nAxonLightOffset + nAxonLights;

		protected const nLDLights:Number = 10;
		protected const nLDLightOffset:Number = nUDLightOffset + nUDLights;

		protected const nLights:Number = nAxonLights + nUDLights + nLDLights;

		protected const nAxonRelays:Number = 24;
		protected const nAxonRelayOffset:Number = 0;

		protected const nSparkleRelays:Number = 5;
		protected const nSparkleRelayOffset:Number = nAxonRelayOffset + nAxonRelays;

		protected const nRelays:Number = nAxonRelays + nSparkleRelays;

		protected const nButtons:Number = 11;

		protected var ledState:Array;
		protected var relayState:Array;

		protected var buttonState:Array;

		private var text3D:Text3D;
		private var textMaterial:Letter3DMaterial;
		private var font3D:Font3D;

		private var lastFrameTime:Number;

		private var eventsRegistered:Boolean = false;

		private var update:Boolean = false;
		private var rotate:Boolean = true;
		private var displayFps:Boolean = false;
		private var displayStructure:Boolean = true;



		public function Soma()
		{
			var i:Number;

			super(1, 1, true, false);

			opaqueBackground = 0;

			createModel();

			ledState = new Array(nLights);
			relayState = new Array(nRelays);
			buttonState = new Array(nButtons);

			for (i = 0; i < nButtons; i++) {
				buttonState[i] = new Boolean(false);
			}

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

		private function buttonPress(n:Number, down:Boolean):void
		{
			var loader:URLLoader;
			var request:URLRequest;

			if (buttonState[n] == down) {
				return;
			}
			buttonState[n] = down;
			request = new URLRequest("http://127.0.0.1:8080/soma/button/" + n +
						 (down ? "/down" : "/up"));
			loader = new URLLoader();
// 			loader.addEventListener(Event.COMPLETE, completeHandler);
// 			loader.addEventListener(Event.OPEN, openHandler);
// 			loader.addEventListener(ProgressEvent.PROGRESS, progressHandler);
// 			loader.addEventListener(SecurityErrorEvent.SECURITY_ERROR, securityErrorHandler);
// 			loader.addEventListener(HTTPStatusEvent.HTTP_STATUS, httpStatusHandler);
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

					if (index < nLights) {
						ledState[index] = val;
					} else if (index >= 0x80 &&
						   ((index - 0x80) < nRelays)) {
						relayState[index - 0x80] = val;
					}
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
			lowerDodeca.roll(36);


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

			textMaterial = new Letter3DMaterial(0x00FFFF);
			font3D = new HelveticaRoman();
			text3D = new Text3D("0 fps", font3D, textMaterial);
			text3D.scale = 0.5;
			text3D.x = 800;
			text3D.y = 600;
			text3D.visible = displayFps;
			scene.addChild(text3D);

		}

		override protected function onRenderTick(event:Event=null):void
		{
			var i:Number;
			var time:Number;
			var date:Date;

			if (!eventsRegistered) {
				stage.addEventListener(KeyboardEvent.KEY_DOWN, onKeyDown);
				stage.addEventListener(KeyboardEvent.KEY_UP, onKeyUp);
				stage.scaleMode = StageScaleMode.SHOW_ALL;
				eventsRegistered = true;
			}

			date = new Date();

			time = date.time;

			text3D.text = (1000 / (time - lastFrameTime)).toFixed(2) + " fps";
			lastFrameTime = time;

			if (update) {
				for (i = 0; i < nAxonLights; i++) {
					axon.setLight(i,ledState[i]);
				}
				for (i = 0; i < nUDLights; i++) {
					upperDodeca.setLight(i,ledState[i + nUDLightOffset]);
				}
				for (i = 0; i < nLDLights; i++) {
					lowerDodeca.setLight(i,ledState[i + nLDLightOffset]);
				}

				for (i = 0; i < nAxonRelays; i++) {
					axon.setRelay(i, relayState[i + nAxonRelayOffset] != 0);
				}

			}

			axon.displayStructure(displayStructure);
			upperDodeca.displayStructure(displayStructure);
			lowerDodeca.displayStructure(displayStructure);
			text3D.visible = displayFps;

			if (rotate)
				soma.yaw(1);

			var scale:Number = stage.height/480;

			super.onRenderTick(event);
		}

		private function onKeyDown(e:KeyboardEvent):void
		{
			trace("keyDown: " + e.keyCode);
			switch (e.keyCode) {
			case 48: /* 0 */
			case 49: /* 1 */
			case 50: /* 2 */
			case 51: /* 3 */
			case 52: /* 4 */
			case 53: /* 5 */
			case 54: /* 6 */
			case 55: /* 7 */
			case 56: /* 8 */
			case 57: /* 9 */
				buttonPress(e.keyCode - 48,true);
				break;

			case 70: /* F */
				if (stage.displayState == StageDisplayState.FULL_SCREEN) {
					stage.displayState = StageDisplayState.NORMAL;
				} else {
					stage.displayState = StageDisplayState.FULL_SCREEN;
				}
				break;

			case 80: /* P */
				displayFps = !displayFps;
				break;

			case 82: /* R */
				rotate = !rotate;
				break;

			case 83: /* S */
				displayStructure = !displayStructure;
				break;

			case 85: /* U */
				update = !update;
				break;
			}
		}
		private function onKeyUp(e:KeyboardEvent):void
		{
			trace("keyUp: " + e.keyCode);
			switch (e.keyCode) {
			case 48: /* 0 */
			case 49: /* 1 */
			case 50: /* 2 */
			case 51: /* 3 */
			case 52: /* 4 */
			case 53: /* 5 */
			case 54: /* 6 */
			case 55: /* 7 */
			case 56: /* 8 */
			case 57: /* 9 */
				buttonPress(e.keyCode - 48,false);
				break;
			}
		}
	}
}
