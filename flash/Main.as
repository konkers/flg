package {
	import org.flg.soma.Soma;

	import flash.display.MovieClip;

	public class Main extends MovieClip
	{

		protected var soma:Soma;

		public function Main()
		{
			stage.frameRate = 30;
			soma = new Soma();
			addChild(soma);
		}
	}
}
