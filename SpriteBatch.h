﻿/*
Аналог SpriteBatch (XNA, DirectXTK) и D3DXSprite.

Существующие проблемы:
- Текст не вращается, пока работает только ttf-шрифт (нужно передать дефайны для SDF в шейдер).
- Нет масштабирования.
- Нет реализована зеркальность.
- Может быть изменен порядок аргументов в функциях.
- Нет режимов сортировки спрайтов перед рендерингов (возможно и не будет). Спрайты выводятся
  в том порядке, в котором были добавлены на отрисовку.

Использование:
В функции Start():
spriteBatch_ = new SpriteBatch(context_);

В обработчике HandleEndViewRender:
spriteBatch_->Begin();
spriteBatch_->Draw(texture, Vector2(100, 100), nullptr, Color::WHITE, Vector2(20, 20), 180.0f);
spriteBatch_->End();
В этом случае SpriteBatch будет под интерфейсом.

Если использовать E_ENDRENDERING, то SpriteBatch будет рисоваться поверх UI.
Пригодится, например, для вывода куросра.
*/


#pragma once

#include <Urho3D/Core/Object.h>
#include <Urho3D/Graphics/GraphicsDefs.h>
#include <Urho3D/Graphics/ShaderVariation.h>

using namespace Urho3D;

namespace Urho3D
{

//class Graphics;
class IndexBuffer;
class Font;
class Texture2D;
class Camera;
class VertexBuffer;

// Режимы зеркального отображения спрайтов.
enum SBEffects
{
    // Спрайт выводится как есть.
    SBE_NONE = 0,

    // Спрайт рисуется повернутым на 180 градусов вокруг вертикальной оси.
    SBE_FLIP_HORIZONTALLY = 1,

    // Спрайт рисуется повернутым на 180 градусов вокруг горизонтальной оси.
    SBE_FLIP_VERTICALLY = 2,

    // Комбинация из двух предыдущих режимов.
    SBE_FLIP_BOTH = SBE_FLIP_HORIZONTALLY | SBE_FLIP_VERTICALLY,
};

class URHO3D_API SpriteBatch : public Object
{
    URHO3D_OBJECT(SpriteBatch, Object);

public:

    SpriteBatch(Context *context);
    virtual ~SpriteBatch();

    // Если указать камеру, то SpriteBatch будет рендериться в мировых координатах.
    void Begin(BlendMode blendMode = BLEND_ALPHA, CompareMode compareMode = CMP_ALWAYS, float z = 0.0f, Camera* camera = nullptr);

    void End();

    void Draw(Texture2D* texture, Rect& destination, Rect* source = nullptr,
        Color color = Color::WHITE, Vector2 origin = Vector2::ZERO, float rotation = 0.0f, SBEffects effects = SBE_NONE);

    void Draw(Texture2D* texture, Vector2& position, Rect* source = nullptr,
        Color color = Color::WHITE, Vector2 origin = Vector2::ZERO, float rotation = 0.0f, SBEffects effects = SBE_NONE);

    void DrawString(String text, Vector2& position, Font* font, int fontSize = 20, Color color = Color::WHITE,
        float rotation = 0.0f, Vector2 origin = Vector2::ZERO, SBEffects effects = SBE_NONE);

private:

    // Отдельный спрайт в очереди на отрисовку.
    struct SBSprite
    {
        Rect source_;
        Rect destination_;
        Color color_;
        Vector2 origin_;
        float rotation_;
        Texture2D* texture_;
        SBEffects effects_;

        // Для отрисовки текста и обычных спрайтов нужны разные шейдеры.
        ShaderVariation* vertexShader_;
        ShaderVariation* pixelShader_;
    };

    // Индексный буфер создается и заполняется один раз, а потом только используется.
    SharedPtr<IndexBuffer> indexBuffer_;

    SharedPtr<VertexBuffer> vertexBuffer_;

    // Спрайты, которые ожидают рендеринга.
    PODVector<SBSprite> sprites_;

    // Режим наложения.
    BlendMode blendMode_;

    // Если использовать CMP_LESSEQUAL, то модели, которые ближе к камере,
    // не будут перекрыты спрайтами.
    CompareMode compareMode_;

    float z_;

    // Если определена камера, то SpriteBatch рендерится в мировых координатах.
    Camera* camera_;

    // Рендерит порцию спрайтов, использующих одну и ту же текстуру и шейдер.
    void RenderPortion(unsigned start, unsigned count);

    // Определяет количество спрайтов, которые можно отренедерить без
    // смены текстуры и шейдера.
    unsigned GetPortionLength(unsigned start);

    // Если определена камера, то спрайты будут отрендерены в мировых координатах,
    // иначе - в экранных.
    Matrix4 GetViewProjMatrix();
};

}