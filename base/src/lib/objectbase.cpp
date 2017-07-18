

#include "gplib.h"
#include "objectbase.h"

#include "ci_ext/vec3.hpp"
#include "ci_ext/Polygon.hpp"
using namespace objectbase;
using namespace gplib;
namespace objectbase
{
  Vec3f::Vec3f(){
    x = 0.f; y = 0.f; z = 0.f;
  }
  Vec3f::Vec3f(float x, float y, float z){
    this->x = x;
    this->y = y;
    this->z = z;
  }
  BaseData::BaseData(){}
  BaseData::BaseData(int w, int h, const std::string resname)
  {
    wid = w;
    hgt = h;
    this->resname = resname;
  }

  //------------------------------------------------------
  //CharBase利用時汎用処理
  //----------------------
  //アニメーション設定
  //max : ０に戻る枚数
  //speed : 切替までのフレーム
  void Anim::set(int max, int speed)
  {
    cnt_ = 0.f;
    max_ = max;
    speed_ = speed;
    no_ = 0;
    flag_ = false;
  }

  //アニメ更新
  //この関数はnoの値を更新する。noの値は
  //src.xに代入する。
  //アニメの順序を変更したい場合は以下
  bool Anim::step()
  {
    if (!flag_)  return false;
    cnt_++;
    if (cnt_ >= speed_){
      cnt_ = 0;
      no_++;
      if (no_ >= max_){
        no_ = 0;
      }
      return true;
    }
    return false;
  }

  //----------------------
  //アニメーション停止
  //speedを0にするだけなので、再開時は再度SetAnimが必要
  void Anim::stop()
  {
    flag_ = false;
  }

  void Anim::resume()
  {
    flag_ = true;
  }
  //アニメーション開始
  void Anim::start()
  {
    flag_ = true;
    cnt_ = 0.f;
    no_ = 0;
  }

  BoxData::BoxData(int x, int y, int w, int h) :
    x_(x),
    y_(y),
    w_(w),
    h_(h)
  {
    rt_.left = x;
    rt_.right = x + w;
    rt_.top = y;
    rt_.bottom = y + h;

    naka_color_ = ARGB(255, 0, 0, 0);
  }
  void BoxData::draw()
  {
    draw::Box(x_, y_, x_ + w_, y_ + h_, 0.f, naka_color_, ARGB(255, 255, 0, 0), 1, 1);

  }
  void BoxData::changeNakaColor(int alpha, int red, int green, int blue)
  {
    naka_color_ = ((alpha & 0xff) << 24) + ((red & 0xff) << 16) + ((green & 0xff) << 8) + blue;
  }

  //------------------------------------------------------
  //CharBase初期化処理
  //
  /*
  //基本となるチップのサイズと利用する画像
  BaseData base = {CHARWID,CHARHGT,charIMG};
  //初期化　指定外の値は初期値になる。
  //この場合はwid,hgt,img以外は初期値
  InitCharBase(player.pos,base);

  //この場合はwid,hgt,img,x,y以外は初期値
  InitCharBase(player.pos,base,STARTPOSX,STARTPOSY);

  利用する際は可視フラグshowも偽になっているため、
  UseCharで可視状態にすること。
  */
  void ObjData::init(const BaseData& base,
    float x, float y, float z, float addx, float addy, float addz,
    int srcX, int srcY, int srcZ, float angle, float scaleX, float scaleY, float scaleZ,
    float alpha, float red, float green, float blue)
  {
    width = base.wid;
    height = base.hgt;
    resname = base.resname;

    pos.x = x;
    pos.y = y;
    pos.z = z;
    add.x = addx;
    add.y = addy;
    add.z = addz;
    scale.x = scaleX;
    scale.y = scaleY;
    scale.z = scaleZ;
    this->angle = angle;
    src.x = (float)srcX;
    src.y = (float)srcY;
    src.z = (float)srcZ;

    a = alpha;
    r = red;
    g = green;
    b = blue;

    show = false;
  }

  void ObjData::init(float x, float y, float z, float addx, float addy, float addz,
    const std::string& srcResName, int srcX, int srcY, int srcZ, int w, int h,
    float angle, float scaleX, float scaleY, float scaleZ,
    float alpha, float red, float green, float blue)
  {
    pos.x = x;
    pos.y = y;
    pos.z = z;
    add.x = addx;
    add.y = addy;
    add.z = addz;
    scale.x = scaleX;
    scale.y = scaleY;
    scale.z = scaleZ;
    src.x = (float)srcX;
    src.y = (float)srcY;
    src.z = (float)srcZ;
    resname = srcResName;
    width = w;
    height = h;
    this->angle = angle;
    a = alpha;
    r = red;
    g = green;
    b = blue;
    show = false;

  }

  //------------------------------------------------------
  //CharBase初期化処理
  //すべて０にする。
  void ObjData::init()
  {
    init(0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
      "", 0, 0, 0, 0, 0,
      0.f, 0.f, 0.f, 0.f,
      0.f, 0.f, 0.f, 0.f);
  }

  //利用チェック
  bool ObjData::checkUse() const
  {
    return (show == true);
  }
  bool ObjData::getUse() const { return show; }



  //汎用移動
  void ObjData::move()
  {
    pos.x += add.x;
    pos.y += add.y;
    pos.z += add.z;
  }

  //----------------------
  //汎用消去処理
  void ObjData::setUseOff()
  {
    show = false;
  }

  //----------------------
  //汎用出現処理
  void ObjData::setUseOn()
  {
    show = true;
  }

  void ObjData::set(bool flag)
  {
    show = flag;
  }

  //----------------------
  //判定用矩形作成 中央座標基準　拡大率考慮
  //大きさがクリッピングサイズと異なる場合に利用する。
  //startx,starty : 左上点からのオフセット値、値分左上から加算される
  //endx,endy : 右下点からのオフセット値、値分右下から減算される
  void ObjData::makeRectScales(RECT &rt, int startx, int starty, int endx, int endy) const
  {
    SetRect(&rt, (int)pos.x - (int)((width*scale.x) / 2) + startx, (int)pos.y - (int)((height * scale.y) / 2) + starty,
      (int)pos.x + (int)((width*scale.x) / 2) - endx, (int)pos.y + (int)((height * scale.y) / 2) - endy);
  }

  RECT ObjData::makeRectScales(int startx, int starty, int endx, int endy) const
  {
    RECT rt;
    SetRect(&rt, (int)pos.x - (int)((width*scale.x) / 2) + startx, (int)pos.y - (int)((height * scale.y) / 2) + starty,
      (int)pos.x + (int)((width*scale.x) / 2) - endx, (int)pos.y + (int)((height * scale.y) / 2) - endy);
    return rt;
  }

  //----------------------
  //判定用矩形作成 左上座標基準
  //大きさがクリッピングサイズと異なる場合に利用する。
  //startx,starty : 左上点からのオフセット値、値分左上から加算される
  //endx,endy : 右下点からのオフセット値、値分右下から減算される
  void ObjData::makeRectLeftTop(RECT &rt, int startx, int starty, int endx, int endy) const
  {
    SetRect(&rt, (int)pos.x + startx, (int)pos.y + starty,
      (int)pos.x + width - endx, (int)pos.y + height - endy);
  }

  RECT ObjData::makeRectLeftTop(int startx, int starty, int endx, int endy) const
  {
    RECT rt;
    SetRect(&rt, (int)pos.x + startx, (int)pos.y + starty,
      (int)pos.x + width - endx, (int)pos.y + height - endy);
    return rt;
  }

  //----------------------
  //判定用矩形作成 中央座標基準
  //大きさがクリッピングサイズと異なる場合に利用する。
  //startx,starty : 左上点からのオフセット値、値分左上から加算される
  //endx,endy : 右下点からのオフセット値、値分右下から減算される
  void ObjData::makeRect(RECT &rt, int startx, int starty, int endx, int endy) const
  {
    SetRect(&rt, (int)pos.x - width / 2 + startx, (int)pos.y - height / 2 + starty,
      (int)pos.x + width / 2 - endx, (int)pos.y + height / 2 - endy);
  }
  RECT ObjData::makeRect(int startx, int starty, int endx, int endy) const
  {
    RECT rt;
    SetRect(&rt, (int)pos.x - width / 2 + startx, (int)pos.y - height / 2 + starty,
      (int)pos.x + width / 2 - endx, (int)pos.y + height / 2 - endy);
    return rt;
  }


  //----------------------
  //汎用描画処理 中央基準
  void ObjData::draw() const
  {
    gplib::draw::Graph(
      pos.x, pos.y, pos.z,
      resname,
      width	* (int)(src.x),
      height * (int)(src.y),
      width, height,
      (int)angle,
      scale.x, scale.y,
      (u_char)a, (u_char)r, (u_char)g, (u_char)b);
  }

  //----------------------
  //汎用描画処理 左上基準
  void ObjData::draw2() const
  {
    draw::GraphLeftTop(
      pos.x, pos.y, pos.z,
      resname,
      width	* (int)(src.x),
      height * (int)(src.y),
      width, height,
      (int)angle, nullptr,
      scale.x, scale.y,
      (u_char)a, (u_char)r, (u_char)g, (u_char)b);
  }


  //----------------------
  //汎用描画処理 中央基準
  void ObjData::drawNC() const
  {

    draw::GraphNC(
      pos.x, pos.y, pos.z,
      resname,
      width	* (int)(src.x),
      height * (int)(src.y),
      width, height,
      (int)angle,
      scale.x, scale.y,
      (u_char)a, (u_char)r, (u_char)g, (u_char)b);
  }

  //----------------------
  //汎用描画処理 左上基準
  void ObjData::draw2NC() const
  {
    draw::GraphLeftTopNC(
      pos.x, pos.y, pos.z,
      resname,
      width	* (int)(src.x),
      height * (int)(src.y),
      width, height,
      (int)angle, nullptr,
      scale.x, scale.y,
      (u_char)a, (u_char)r, (u_char)g, (u_char)b);
  }
  //----------------------
  //あたりチェック
  bool ObjData::checkHit(const RECT& destRt) const
  {
    RECT rt;
    makeRect(rt);
    //デバッグ----------------
    draw::CkRect(destRt);
    draw::CkRect(rt);
    //graph::Draw_CkRect(destRt);
    //graph::Draw_CkRect(rt);
    //ここまで----------------
    if (math::RectCheck(rt, destRt)){
      return true;
    }
    else{
      return false;
    }
  }

  //----------------------
  //あたりチェック 拡大率考慮
  bool ObjData::checkHitScales(const RECT& destRt) const
  {
    RECT rt;
    makeRectScales(rt);
    //デバッグ----------------
    //	Draw_CkRect(rt1);
    //	Draw_CkRect(rt2);
    //ここまで----------------
    if (math::RectCheck(rt, destRt)){
      return true;
    }
    else{
      return false;
    }
  }
  //----------------------
  //あたりチェック
  bool ObjData::checkHit(ObjData& destObj) const
  {
    RECT srcRt;
    makeRect(srcRt);

    RECT destRt = destObj.makeRect();
    //デバッグ----------------
    draw::CkRect(destRt);
    draw::CkRect(srcRt);
    //graph::Draw_CkRect(destRt);
    //graph::Draw_CkRect(rt);
    //ここまで----------------
    if (math::RectCheck(srcRt, destRt)){
      return true;
    }
    else{
      return false;
    }
  }

  //----------------------
  //あたりチェック 拡大率考慮
  bool ObjData::checkHitScales(ObjData& destObj) const
  {
    RECT srcRt;
    makeRectScales(srcRt);

    RECT destRt;
    destObj.makeRectScales(destRt);
    //デバッグ----------------
    //	Draw_CkRect(rt1);
    //	Draw_CkRect(rt2);
    //ここまで----------------
    if (math::RectCheck(srcRt, destRt)){
      return true;
    }
    else{
      return false;
    }
  }
  //----------------------
  //枠描画
  //  cb_ charbase情報
  //  color 色情報（ARGBでも指定可能）
  //  lineweight ラインの太さ
  void ObjData::Draw_Frame(ci_ext::Color color, int lineweight) const
  {
    //ci_ext::Polygon<float> a(
    //  ci_ext::Vec3<float>(pos.x, pos.y, pos.z), //pos
    //  {
    //    ci_ext::Vec3<float>(-width / 2.0f, -height / 2.0f, 0.0f),
    //    ci_ext::Vec3<float>(-width / 2.0f, height / 2.0f, 0.0f),
    //    ci_ext::Vec3<float>(width / 2.0f, height / 2.0f, 0.0f),
    //    ci_ext::Vec3<float>(width / 2.0f, -height / 2.0f, 0.0f),
    //  },//vertex
    //  angle,//degree
    //  ci_ext::Vec3<float>(scale.x, scale.y, 1.0f)//scale
    //  );

    //a.draw(color, lineweight);
  }
  //----------------------
  //あたりチェック
  bool ObjData::checkHit2(const ObjData& cb, bool debug_mode) const
  {
    ////ポリゴン作って判定させる
    //ci_ext::Polygon<float> a(
    //  ci_ext::Vec3<float>(pos.x, pos.y, pos.z), //pos
    //  {
    //    ci_ext::Vec3<float>(-width / 2.0f, -height / 2.0f, 0.0f),
    //    ci_ext::Vec3<float>(-width / 2.0f, height / 2.0f, 0.0f),
    //    ci_ext::Vec3<float>(width / 2.0f, height / 2.0f, 0.0f),
    //    ci_ext::Vec3<float>(width / 2.0f, -height / 2.0f, 0.0f),
    //  },//vertex
    //  angle,//degree
    //  ci_ext::Vec3<float>(scale.x, scale.y, 1.0f)//scale
    //  );

    //ci_ext::Polygon<float> b(
    //  ci_ext::Vec3<float>(cb.pos.x, cb.pos.y, cb.pos.z),
    //  {
    //    ci_ext::Vec3<float>(-cb.width / 2.0f, -cb.height / 2.0f, 0.0f),
    //    ci_ext::Vec3<float>(-cb.width / 2.0f, cb.height / 2.0f, 0.0f),
    //    ci_ext::Vec3<float>(cb.width / 2.0f, cb.height / 2.0f, 0.0f),
    //    ci_ext::Vec3<float>(cb.width / 2.0f, -cb.height / 2.0f, 0.0f),
    //  },
    //  cb.angle,
    //  ci_ext::Vec3<float>(cb.scale.x, cb.scale.y, 1.0f)
    //  );

    //if (debug_mode)
    //{
    //  Draw_Frame(0xFFFF0000, 1);
    //  cb.Draw_Frame(0xFFFF0000, 1);
    //}
    return false;//a.intersect(b);
  }
  /*
    //----------------------
    //あたりチェック
    bool ObjData::HitCheck2(const ObjData& cb1, const ObjData& cb2)
    {
    std::vector<ci_ext::Vec3<float>> v1 = {
    ci_ext::Vec3<float>(-cb1.width / 2.0f, -cb1.height / 2.0f, 0.0f),
    ci_ext::Vec3<float>(-cb1.width / 2.0f, cb1.height / 2.0f, 0.0f),
    ci_ext::Vec3<float>(cb1.width / 2.0f, cb1.height / 2.0f, 0.0f),
    ci_ext::Vec3<float>(cb1.width / 2.0f, -cb1.height / 2.0f, 0.0f),
    };
    std::vector<ci_ext::Vec3<float>> v2 = {
    ci_ext::Vec3<float>(-cb2.width / 2.0f, -cb2.height / 2.0f, 0.0f),
    ci_ext::Vec3<float>(-cb2.width / 2.0f, cb2.height / 2.0f, 0.0f),
    ci_ext::Vec3<float>(cb2.width / 2.0f, cb2.height / 2.0f, 0.0f),
    ci_ext::Vec3<float>(cb2.width / 2.0f, -cb2.height / 2.0f, 0.0f),
    };
    //ポリゴン作って判定させる
    ci_ext::Polygon<float> a(
    ci_ext::Vec3<float>(cb1.pos.x, cb1.pos.y, cb1.pos.z),
    v1,//vertex
    cb1.angle,
    ci_ext::Vec3<float>(cb1.scale.x, cb1.scale.y, 1.0f));//scale
    ci_ext::Polygon<float> b(
    ci_ext::Vec3<float>(cb2.pos.x, cb2.pos.y, cb2.pos.z),
    v2,
    cb2.angle,
    ci_ext::Vec3<float>(cb2.scale.x, cb2.scale.y, 1.0f)
    );
    if (a.intersect(b))
    {
    return true;
    }
    return false;
    }

    //----------------------
    //枠描画
    //  cb_ charbase情報
    //  color 色情報（ARGBでも指定可能）
    //  lineweight ラインの太さ
    void ObjData::Draw_Frame(const ObjData& cb, ci_ext::Color color, int lineweight)
    {
    std::vector<ci_ext::Vec3<float>> v1 = {
    ci_ext::Vec3<float>(-cb.width / 2.0f, -cb.height / 2.0f, 0.0f),
    ci_ext::Vec3<float>(-cb.width / 2.0f, cb.height / 2.0f, 0.0f),
    ci_ext::Vec3<float>(cb.width / 2.0f, cb.height / 2.0f, 0.0f),
    ci_ext::Vec3<float>(cb.width / 2.0f, -cb.height / 2.0f, 0.0f),
    };
    ci_ext::Polygon<float> a(
    ci_ext::Vec3<float>(cb.pos.x, cb.pos.y, cb.pos.z), //pos
    v1,//vertex
    cb.angle,//degree
    ci_ext::Vec3<float>(cb.scale.x, cb.scale.y, 1.0f)//scale
    );

    a.draw(color, lineweight);
    }
    */
}
//ここまで
//------------------------------------------------------
