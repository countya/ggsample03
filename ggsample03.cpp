//
// ゲームグラフィックス特論宿題アプリケーション
//
#include "GgApp.h"

// プロジェクト名
#ifndef PROJECT_NAME
#  define PROJECT_NAME "ggsample03"
#endif

// オブジェクト関連の処理
#include "object.h"

// シェーダー関連の処理
#include "shader.h"

//
// 4 行 4 列の行列の積を求める
//
//   m ← m1 × m2
//
static void multiply(GLfloat* m, const GLfloat* m1, const GLfloat* m2)
{
  for (int i = 0; i < 16; ++i)
  {
    int j = i & 3, k = i & ~3;

    // 配列変数に行列が転置された状態で格納されていることを考慮している
    m[i] = m1[0 + j] * m2[k + 0] + m1[4 + j] * m2[k + 1] + m1[8 + j] * m2[k + 2] + m1[12 + j] * m2[k + 3];
  }
}

//
// 単位行列を設定する
//
//   m: 単位行列を格納する配列
//
static void loadIdentity(GLfloat* m)
{
  m[ 0] = m[ 5] = m[10] = m[15] = 1.0f;
  m[ 1] = m[ 2] = m[ 3] = m[ 4] =
  m[ 6] = m[ 7] = m[ 8] = m[ 9] =
  m[11] = m[12] = m[13] = m[14] = 0.0f;
}

//
// 直交投影変換行列を求める
//
//   m: 直交投影変換行列を格納する配列
//   left, right: ビューボリュームの左右端
//   bottom, top: ビューボリュームの上下端
//   zNear, zFar: 前方面および後方面までの距離
//
static void ortho(GLfloat* m, float left, float right, float bottom, float top, float zNear, float zFar)
{
  m[ 0] = 2.0f / (right - left);
  m[ 5] = 2.0f / (top - bottom);
  m[10] = -2.0f / (zFar - zNear);
  m[12] = -(right + left) / (right - left);
  m[13] = -(top + bottom) / (top - bottom);
  m[14] = -(zFar + zNear) / (zFar - zNear);
  m[15] = 1.0f;
  m[ 1] = m[ 2] = m[ 3] = m[ 4] = m[ 6] = m[ 7] = m[ 8] = m[ 9] = m[11] = 0.0f;
}

//
// 透視投影変換行列を求める
//
//   m: 透視投影変換行列を格納する配列
//   left, right: 前方面の左右端
//   bottom, top: 前方面の上下端
//   zNear, zFar: 前方面および後方面までの距離
//
static void frustum(GLfloat* m, float left, float right, float bottom, float top, float zNear, float zFar)
{
  m[ 0] = 2.0f * zNear / (right - left);
  m[ 5] = 2.0f * zNear / (top - bottom);
  m[ 8] = (right + left) / (right - left);
  m[ 9] = (top + bottom) / (top - bottom);
  m[10] = -(zFar + zNear) / (zFar - zNear);
  m[11] = -1.0f;
  m[14] = -2.0f * zFar * zNear / (zFar - zNear);
  m[ 1] = m[ 2] = m[ 3] = m[ 4] = m[ 6] = m[ 7] = m[12] = m[13] = m[15] = 0.0f;
}

//
// 画角を指定して透視投影変換行列を求める
//
//   m: 透視投影変換行列を格納する配列
//   fovy: 画角（ラジアン）
//   aspect: ウィンドウの縦横比
//   zNear, zFar: 前方面および後方面までの距離
//
static void perspective(GLfloat* m, float fovy, float aspect, float zNear, float zFar)
{
  // 【宿題】ここを解答してください（loadIdentity() を置き換えてください）
  //透視投影変換行列を求めて引数の配列変数 m に格納する関数
  loadIdentity(m);
  //ここがおかしいかも1
  GLfloat PL[4];
  PL[0] = 1;
  PL[1] = 1;
  PL[2] = 1;
  PL[3] = 1;
  
  loadIdentity(PL);

  const GLfloat f =1 / tan(fovy / 2);
  std::cout << "fovy is" << fovy << "--" << "f is" << f << std::endl;
  /*
  GLfloat T[] = {
    f/aspect,0,0,0,
    0,zFar,0,0,
    0,0,(zFar+zNear)/(zFar-zNear),-2*zFar*zNear/(zFar-zNear),
    0,0,-1,0
  };
  multiply(m, T,PL);
  */
  m[0] = f / aspect;
  m[1] = m[2] = m[3] = 0.0f;
  m[4] = m[6] = m[7] = 0.0f;
  m[5] = f;
  m[8] = m[9] = 0.0f;
  m[10] = -(zFar + zNear) / (zFar - zNear);
  m[11] = -1.0f;
  m[12] = m[13] = m[15] = 0.0f;
  m[14] = -2 * zFar * zNear / (zFar - zNear);

}

//
// ビュー変換行列を求める
//
//   m: ビュー変換行列を格納する配列
//   ex, ey, ez: 視点の位置
//   tx, ty, tz: 目標点の位置
//   ux, uy, uz: 上方向のベクトル
//
static void lookat(GLfloat* m, float ex, float ey, float ez, float tx, float ty, float tz, float ux, float uy, float uz)
{
  // 【宿題】ここを解答してください（loadIdentity() を置き換えてください）
  loadIdentity(m);

  GLfloat GF[16];
  loadIdentity(GF);
  GF[12] = -ex;
  GF[13] = -ey;
  GF[14] = -ez;


  const GLfloat t[] = {
    ex - tx,
    ey - ty,
    ez - tz,
  };
    const GLfloat u[] = {
      uy * t[2] - uz *t[1],
      uz * t[0] - ux *t[2],
      ux * t[1] - uy *t[0]
  };


    const GLfloat s[] = {
      t[1] * u[2] - t[2] * u[1],
      t[2] * u[0] - t[0] * u[2],
      t[0] * u[1] - t[1] * u[0]
    };

    const GLfloat rlength = ggLength3(u);
    const GLfloat tlength = ggLength3(t);
    const GLfloat slength = ggLength3(s);

    GLfloat S[] = {
      u[0] / rlength, s[0] / slength, t[2] / tlength,0,
      u[1] / rlength, s[1] / slength, t[2] / tlength,0,
      u[2] / rlength, s[2] / slength, t[2] / tlength,0,
      0,0,0,1
    };
    multiply(m, S, GF);
}

//
// アプリケーション本体
//
int GgApp::main(int argc, const char* const* argv)
{
  // ウィンドウを作成する (この行は変更しないでください)
  Window window{ argc > 1 ? argv[1] : PROJECT_NAME };

  // 背景色を指定する
  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

  // プログラムオブジェクトの作成
  const auto program{ loadProgram(PROJECT_NAME ".vert", "pv", PROJECT_NAME ".frag", "fc") };

  // uniform 変数のインデックスの検索（見つからなければ -1）
  const auto mcLoc{ glGetUniformLocation(program, "mc") };

  // ビュー変換行列を mv に求める
  GLfloat mv[16];
  lookat(mv, 3.0f, 4.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

  // 頂点属性
  static const GLfloat position[][3]
  {
    { -0.9f, -0.9f, -0.9f },  // (0)
    {  0.9f, -0.9f, -0.9f },  // (1)
    {  0.9f, -0.9f,  0.9f },  // (2)
    { -0.9f, -0.9f,  0.9f },  // (3)
    { -0.9f,  0.9f, -0.9f },  // (4)
    {  0.9f,  0.9f, -0.9f },  // (5)
    {  0.9f,  0.9f,  0.9f },  // (6)
    { -0.9f,  0.9f,  0.9f },  // (7)
  };

  // 頂点数
  constexpr auto vertices{ static_cast<GLuint>(std::size(position)) };

  // 頂点インデックス
  static const GLuint index[]
  {
    0, 1,
    1, 2,
    2, 3,
    3, 0,
    0, 4,
    1, 5,
    2, 6,
    3, 7,
    4, 5,
    5, 6,
    6, 7,
    7, 4,
  };

  // 稜線数
  constexpr auto lines{ static_cast<GLuint>(std::size(index)) };

  // 頂点配列オブジェクトの作成
  const auto vao{ createObject(vertices, position, lines, index) };

  // ウィンドウが開いている間繰り返す
  while (window)
  {
    // ウィンドウを消去する
    glClear(GL_COLOR_BUFFER_BIT);

    // シェーダプログラムの使用開始
    glUseProgram(program);

    // 投影変換行列 mp を求める (window.getAspect() はウィンドウの縦横比)
    GLfloat mp[16];
    perspective(mp, 0.5f, window.getAspect(), 1.0f, 15.0f);

    // 投影変換行列 mp とビュー変換行列 mv の積を変換行列 mc に求める
    GLfloat mc[16];
    multiply(mc, mp, mv);

    // uniform 変数 mc に変換行列 mc を設定する
    // 【宿題】ここを解答してください（uniform 変数 mc のインデックスは変数 mcLoc に入っています）
    glUniformMatrix4fv(mcLoc, 1, GL_FALSE, mc);

    // 描画に使う頂点配列オブジェクトの指定
    glBindVertexArray(vao);

    // 図形の描画
    glDrawElements(GL_LINES, lines, GL_UNSIGNED_INT, 0);

    // 頂点配列オブジェクトの指定解除
    glBindVertexArray(0);

    // シェーダプログラムの使用終了
    glUseProgram(0);

    // カラーバッファを入れ替えてイベントを取り出す
    window.swapBuffers();
  }

  return 0;
}
