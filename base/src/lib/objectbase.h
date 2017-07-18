#pragma once
#include "ci_ext\color.hpp"
namespace objectbase{

	//代入可能なベースデータ
	//初期値が0,1.0fのものは除外する。
	struct BaseData{
		int		wid, hgt;		//画像幅高さ
		std::string 	resname;		//使用イメージ名
		BaseData();
    BaseData(int, int, const std::string);
	};

	struct Vec3f{
		float x, y, z;
		Vec3f();
		Vec3f(float,float,float);
	};

	//
	//
	//
	//
	class Anim{
	private:
		bool  flag_;
		float	cnt_;				//アニメ更新カウンタ
	public:
		int		no_;				//現在のアニメNO　この値は0,1,2,3,4,とカウントするだけ
		int	  speed_;				//アニメ更新速度
		int		max_;				//アニメ枚数　この値にnoが来ると0からカウント
		//----------------------
		//アニメーション設定
		//max : ０に戻る枚数
		//speed : アニメ速度
		void set(int max, int speed);
		//アニメ更新
		bool step();
		//アニメーション停止
		void stop();
		//アニメーション開始 0から
		void start();
		//アニメーション再開
		void resume();
	};

	//矩形描画の汎用処理
	class BoxData
	{
	public:
		int x_, y_, h_, w_;
    RECT rt_;
		D3DCOLOR naka_color_;
    BoxData(int x, int y, int w, int h);
    void draw();
    void changeNakaColor(int alpha, int red, int green, int blue);
	};


	class ObjData
	{
	//本来以下メンバはGETSETでの操作もしくは、触らない構造とすべきだが、
	//すべてのメンバに対して外部より代入を伴う操作をする可能性があるため
	//publicで統一をする。
	//オブジェクト管理用のデータ郡+それらの処理メソッドの提供を行うクラスである
	public:
		Vec3f		pos;				//座標
		Vec3f		add;				//移動量
		int			width, height;		//画像幅高さ

		Vec3f		scale;				//拡大率
		Vec3f		src;				//抽出位置
		std::string resname;			//使用イメージ名

		bool		show;				//利用フラグ

		float		angle;				//角度
		float		a;					//アルファ
		float		r, g, b;			//色
		//----------------------
		//汎用移動処理
		void move();
		//----------------------
		//汎用可視判別処理
    bool getUse() const;
    bool checkUse() const;
		//----------------------
		//汎用消去処理
		void setUseOff();
		//----------------------
		//汎用出現処理
		void setUseOn();
    //----------------------
		//汎用出現消去処理
		void set(bool);
		//----------------------
		//判定用矩形作成 左上座標基準
		//大きさがクリッピングサイズと異なる場合に利用する。
		//startx,starty : 左上点からのオフセット値、値分左上から加算される
		//endx,endy : 右下点からのオフセット値、値分右下から減算される
		void makeRectLeftTop(RECT &rt, int startx = 0, int starty = 0, int endx = 0, int endy = 0)const;
		//----------------------
		//判定用矩形作成 中央座標基準
		//大きさがクリッピングサイズと異なる場合に利用する。
		//startx,starty : 左上点からのオフセット値、値分左上から加算される
		//endx,endy : 右下点からのオフセット値、値分右下から減算される
		void makeRect(RECT &rt, int startx = 0, int starty = 0, int endx = 0, int endy = 0)const;
		//----------------------
		//判定用矩形作成 中央座標基準　拡大率考慮
		//大きさがクリッピングサイズと異なる場合に利用する。
		//startx,starty : 左上点からのオフセット値、値分左上から加算される
		//endx,endy : 右下点からのオフセット値、値分右下から減算される
		void makeRectScales(RECT &rt, int startx = 0, int starty = 0, int endx = 0, int endy = 0)const;
    //----------------------
    //判定用矩形作成 左上座標基準
    //大きさがクリッピングサイズと異なる場合に利用する。
    //startx,starty : 左上点からのオフセット値、値分左上から加算される
    //endx,endy : 右下点からのオフセット値、値分右下から減算される
		RECT makeRectLeftTop(int startx = 0, int starty = 0, int endx = 0, int endy = 0)const;
    //----------------------
    //判定用矩形作成 中央座標基準
    //大きさがクリッピングサイズと異なる場合に利用する。
    //startx,starty : 左上点からのオフセット値、値分左上から加算される
    //endx,endy : 右下点からのオフセット値、値分右下から減算される
    RECT makeRect( int startx = 0, int starty = 0, int endx = 0, int endy = 0) const;
    //----------------------
    //判定用矩形作成 中央座標基準　拡大率考慮
    //大きさがクリッピングサイズと異なる場合に利用する。
    //startx,starty : 左上点からのオフセット値、値分左上から加算される
    //endx,endy : 右下点からのオフセット値、値分右下から減算される
		RECT makeRectScales(int startx = 0, int starty = 0, int endx = 0, int endy = 0) const;
    //----------------------
		//汎用描画処理 中央基準
    void draw() const;
		//----------------------
		//汎用描画処理 左上基準
    void draw2() const;
		//----------------------
		//汎用描画処理 中央基準 カメラ概念無視
    void drawNC() const;
		//----------------------
		//汎用描画処理 左上基準 カメラ概念無視
    void draw2NC() const;
		//----------------------
		//あたりチェック 内部でRECTを生成し調べる
		bool checkHit(const RECT& destRt)const;
		//----------------------
		//あたりチェック 内部でRECTを生成し調べる。拡大率を考慮に入れる
		bool checkHitScales(const RECT& destRt)const;
    //----------------------
    //あたりチェック 内部でRECTを生成し調べる
		bool checkHit(ObjData& destObj)const;
    //----------------------
    //あたりチェック 内部でRECTを生成し調べる。拡大率を考慮に入れる
		bool checkHitScales(ObjData& destObj)const;

    bool checkHit2(const ObjData& cb, bool debug_mode = false) const;
    void Draw_Frame(ci_ext::Color color, int lineweight) const;
      //------------------------------------------------------
		//初期化処理
		/*
		//基本となるチップのサイズと利用する画像
		BaseData base = {CHARWID,CHARHGT,charIMG};
		//初期化　指定外の値は初期値になる。
		//この場合はwid,hgt,img以外は初期値
		Init(player.pos,base);

		//この場合はwid,hgt,img,x,y以外は初期値
		Init(player.pos,base,STARTPOSX,STARTPOSY);

		利用する際は可視フラグshowも偽になっているため、
		Useで可視状態にすること。
		*/
		void init(const BaseData& base,
			float x = 0.f, float y = 0.f, float z = 0.f, float addx = 0.f, float addy = 0.f, float addz = 0.f,
			int srcX = 0, int srcY = 0, int srcZ = 0, float angle = 0.f, float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.f,
			float alpha = 255.f, float red = 255.f, float green = 255.f, float blue = 255.f);

		//------------------------------------------------------
		//初期化処理
		//
		/*
		//初期化　指定外の値は初期値になる。
		利用する際は可視フラグshowも偽になっているため、
		Useで可視状態にすること。
		*/
		void init(float x, float y, float z, float addx, float addy, float addz,
			const std::string& resname, int srcX, int srcY, int srcZ, int w, int h,
			float angle = 0.f, float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f,
			float alpha = 255.f, float red = 255.f, float green = 255.f, float blue = 255.f);

		//------------------------------------------------------
		//初期化処理
		//内部構造体を０クリアする
		void init();
		ObjData(){};
		ObjData(float x, float y, float z, float addx, float addy, float addz,
			const std::string& resname, int srcX, int srcY, int srcZ, int w, int h,
			float angle = 0.f, float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f,
			float alpha = 255.f, float red = 255.f, float green = 255.f, float blue = 255.f);
	};

	


}



