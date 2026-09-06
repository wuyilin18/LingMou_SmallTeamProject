// lib/LingMouEngine/LingMouEngine.h
#pragma once  // 防坑：防止头文件被重复包含

#include <Arduino.h>
#include <TFT_eSPI.h>
// ===== AsyncTimer.h =====
#ifndef _ASYNCTIMER_h
#define _ASYNCTIMER_h

extern TFT_eSprite eyeSprite;  // 桥接到 LingMou.ino 里的全局 Sprite

#include <Arduino.h>

typedef void(*AsyncTimerCallback)();

class AsyncTimer {
 public:
	AsyncTimer(unsigned long millisInterval);
	AsyncTimer(unsigned long millisInterval, AsyncTimerCallback OnFinish);

	void Start();
	void Reset();
	void Stop();
	bool Update();

	void SetIntervalMillis(unsigned long interval);
	
	unsigned long GetStartTime();
	unsigned long GetElapsedTime();
	unsigned long GetRemainingTime();

	bool IsActive() const;
	bool IsExpired() const;
	
	unsigned long Interval;
	
	AsyncTimerCallback OnFinish;

private:
	bool _isActive;
	bool _isExpired;
	unsigned long _startTime;
};
#endif

// ===== Animations.h =====
#ifndef _ANIMATIONS_h
#define _ANIMATIONS_h

#include <Arduino.h>

class IAnimation {
public:
	virtual float GetValue() = 0;
	virtual float GetValue(unsigned long overWriteMillis) = 0;
	virtual unsigned long GetElapsed() = 0;

private:
	virtual float Calculate(unsigned long elapsedMillis) = 0;
};

class AnimationBase : IAnimation {
  public:
	  AnimationBase(unsigned long interval) : Interval(interval), StarTime(millis()) {}

	  unsigned long Interval;
	  unsigned long StarTime;

	  virtual void Restart() {
		  StarTime = millis();
	  }
	  float GetValue() override final {
		  return GetValue(GetElapsed());
	  }
	  float GetValue(unsigned long elapsedMillis) override final {
		  return Calculate(elapsedMillis);
	  }
	  unsigned long GetElapsed() override {
		  return static_cast<unsigned long> (millis() - StarTime);
	  }

  protected:
	  float Calculate(unsigned long elapsedMillis) override { return 0.0; }
};

class DeltaAnimation : public AnimationBase {
  public:
	  unsigned long StarTime;
	  DeltaAnimation(unsigned long interval) : AnimationBase(interval) {};
	  float Calculate(unsigned long elapsedMillis) {
		  if (elapsedMillis < Interval)	{
			  return 0.0f;
		  }
		  else {
			  return 1.0f;
		  }
	  };
};


class StepAnimation : public AnimationBase {
  public:
	  unsigned long Interval;
	  unsigned long StarTime;
	  bool IsActive = true;
  	StepAnimation(unsigned long interval) : AnimationBase(interval) {};

	  float Calculate(unsigned long elapsedMillis) {
		  if (elapsedMillis < Interval)	{
			  return 0.0f;
		  }
		  return 1.0f;
	  };
};


class RampAnimation : public AnimationBase {
public:

	unsigned long StarTime;
	bool IsActive = true;

	RampAnimation(unsigned long interval) : AnimationBase(interval) {};

	float Calculate(unsigned long elapsedMillis) {
		if (elapsedMillis < Interval)	{
			return static_cast<float>(elapsedMillis) / Interval;
		}
		return 1.0f;
	};
};

class TriangleAnimation : public AnimationBase {
public:

	TriangleAnimation(unsigned long interval) : AnimationBase(interval) {
		_t0 = interval / 2;
		_t1 = interval - _t0;
	}
	TriangleAnimation(unsigned long t0, unsigned long t1) : AnimationBase(t0 + t1) {
		_t0 = t0;
		_t1 = t1;
	};
	float Calculate(unsigned long elapsedMillis) {
		if (elapsedMillis % Interval < _t0) {
			return static_cast<float>(elapsedMillis % Interval) / _t0;
		}
		return 1.0f - (static_cast<float>(elapsedMillis % Interval) - _t0) / _t1;
	};
	unsigned long _t0;
	unsigned long _t1;
};


class TrapeziumAnimation : public AnimationBase {
public:
	TrapeziumAnimation(unsigned long t) : AnimationBase(t) {
		_t0 = t / 3;
		_t1 = _t0;
		_t2 = t - _t0 - _t1;
	};
	TrapeziumAnimation(unsigned long t0, unsigned long t1, unsigned long t2) : AnimationBase(t0 + t1 + t2) {
		_t0 = t0;
		_t1 = t1;
		_t2 = t2;
	}; 
	float Calculate(unsigned long elapsedMillis) override {
		if (elapsedMillis > Interval) return 0.0;
		if (elapsedMillis < _t0) {
			return static_cast<float>(elapsedMillis) / _t0;
		}
		else if (elapsedMillis < _t0 + _t1) {
			return 1.0f;
		}
		else {
			return 1.0f - (static_cast<float>(elapsedMillis) - _t1 - _t0) / _t2;
		}
	};

	unsigned long _t0;
	unsigned long _t1;
	unsigned long _t2;
};


class TrapeziumPulseAnimation : public AnimationBase {
public:
	TrapeziumPulseAnimation(unsigned long t) : AnimationBase(t) {
		_t0 = 0;
		_t1 = t / 3;
		_t2 = t - _t0 - _t0;
		_t3 = _t1;
		_t4 = 0;
	};

	TrapeziumPulseAnimation(unsigned long t0, unsigned long t1, unsigned long t2) : AnimationBase(t0 + t1 + t2) {
		_t0 = 0;
		_t1 = t0;
		_t2 = t1;
		_t3 = t2;
		_t4 = 0;
	};

	TrapeziumPulseAnimation(unsigned long t0, unsigned long t1, unsigned long t2, unsigned long t3, unsigned long t4) : AnimationBase(t0 + t1 + t2 + t3 + t4) {
		_t0 = t0;
		_t1 = t1;
		_t2 = t2;
		_t3 = t3;
		_t4 = t4;
	};

	float Calculate(unsigned long elapsedMillis) override {
		unsigned long elapsed = elapsedMillis % Interval;

		if (elapsed < _t0) {
			return 0.0;
		}
		if (elapsed < _t0 + _t1) {
			return static_cast<float>(elapsed - _t0) / _t1;
		}
		else if (elapsed < _t0 + _t1 + _t2)	{
			return 1.0f;
		}
		else if (elapsed < _t0 + _t1 + _t2 + _t3)	{
			return 1.0f - (static_cast<float>(elapsed) - _t2 - _t1 - _t0) / _t3;
		}
		return 0.0;
	};

	void SetInterval(uint16_t t) {
		_t0 = 0;
		_t1 = t / 3;
		_t2 = t - _t0 - _t0;
		_t3 = _t1;
		_t4 = 0;
		Interval = _t0 + _t1 + _t2 + _t3 + _t4;
	}

	void SetTriangle(uint16_t t, uint16_t delay) {
		_t0 = 0;
		_t1 = t / 2;
		_t2 = 0;
		_t3 = _t1;
		_t4 = delay;
		Interval = _t0 + _t1 + _t2 + _t3 + _t4;
	}

	void SetTriangleCuadrature(uint16_t t, uint16_t delay) {
		_t0 = delay;
		_t1 = t / 2;
		_t2 = 0;
		_t3 = _t1;
		_t4 = 0;
		Interval = _t0 + _t1 + _t2 + _t3 + _t4;
	}
	
	void SetPulse(uint16_t t, uint16_t delay) {
		_t0 = 0;
		_t1 = t / 3;
		_t2 = t - _t0 - _t0;
		_t3 = _t1;
		_t4 = delay;
		Interval = _t0 + _t1 + _t2 + _t3 + _t4;
	}

	void SetPulseCuadrature(uint16_t t, uint16_t delay) {
		_t0 = delay;
		_t1 = t / 3;
		_t2 = t - _t0 - _t0;
		_t3 = _t1;
		_t4 = 0;
		Interval = _t0 + _t1 + _t2 + _t3 + _t4;
	}

	void SetInterval(uint16_t t0, uint16_t t1, uint16_t t2, uint16_t t3, uint16_t t4) {
		_t0 = t0;
		_t1 = t1;
		_t2 = t2;
		_t3 = t3;
		_t4 = t4;
		Interval = _t0 + _t1 + _t2 + _t3 + _t4;
	}

	unsigned long _t0;
	unsigned long _t1;
	unsigned long _t2;
	unsigned long _t3;
	unsigned long _t4;
};

#endif

// ===== EyeConfig.h =====
#ifndef _EYECONFIG_h
#define _EYECONFIG_h

struct EyeConfig
{
    int16_t OffsetX;
    int16_t OffsetY;

    int16_t Height;
    int16_t Width;

    float Slope_Top;
    float Slope_Bottom;

    int16_t Radius_Top;
    int16_t Radius_Bottom;

    int16_t Inverse_Radius_Top;
    int16_t Inverse_Radius_Bottom;

    int16_t Inverse_Offset_Top;
    int16_t Inverse_Offset_Bottom;

    uint8_t Color = 1;
};


#endif

// ===== EyeTransition.h =====
#ifndef _EYETRANSITION_h
#define _EYETRANSITION_h

#include <Arduino.h>

class EyeTransition {
public:
	EyeTransition();

	EyeConfig* Origin;
	EyeConfig Destin;

	RampAnimation Animation;

	void Update();
	void Apply(float t);
};

#endif

// ===== EyeTransformation.h =====
#ifndef _EYETRANSFORMATION_h
#define _EYETRANSFORMATION_h

#include <Arduino.h>

struct Transformation
{
	float MoveX = 0.0;
	float MoveY = 0.0;
	float ScaleX = 1.0;
	float ScaleY = 1.0;
};

class EyeTransformation
{
public:
	EyeTransformation();

	EyeConfig* Input;
	EyeConfig Output;

	Transformation Origin;
	Transformation Current;
	Transformation Destin;

	RampAnimation Animation;

	void Update();
	void Apply();
	void SetDestin(Transformation transformation);
};

#endif

// ===== EyeVariation.h =====
#ifndef _EYEVARIATION_h
#define _EYEVARIATION_h

#include <Arduino.h>

class EyeVariation {
public:
	EyeVariation();

	EyeConfig* Input;
	EyeConfig Output;

	TrapeziumPulseAnimation Animation;

	EyeConfig Values;
	void Clear();

	void SetInterval(uint16_t t0, uint16_t t1, uint16_t t2, uint16_t t3, uint16_t t4);

	void Update();
	void Apply(float t);
};

#endif

// ===== EyeBlink.h =====
#ifndef _EYEBLINK_h
#define _EYEBLINK_h

#include <Arduino.h>

class EyeBlink {
 protected:

public:
	EyeBlink();

	EyeConfig* Input;
	EyeConfig Output;

	TrapeziumAnimation Animation;

	int32_t BlinkWidth = 60;
	int32_t BlinkHeight = 2;

	void Update();
	void Apply(float t);
};

#endif

// ===== EyeDrawer.h =====
#ifndef _EYEDRAWER_h
#define _EYEDRAWER_h


enum CornerType {T_R, T_L, B_L, B_R};

/**
 * Contains all functions to draw eye based on supplied (expression-based) config
 */
class EyeDrawer {
  public:
    static void Draw(int16_t centerX, int16_t centerY, EyeConfig *config) {
      int32_t delta_y_top    = config->Height * config->Slope_Top / 2.0;
      int32_t delta_y_bottom = config->Height * config->Slope_Bottom / 2.0;
      auto totalHeight = config->Height + delta_y_top - delta_y_bottom;
      if (config->Radius_Bottom > 0 && config->Radius_Top > 0 &&
          totalHeight - 1 < config->Radius_Bottom + config->Radius_Top) {
        int32_t corrected_radius_top    = (float)config->Radius_Top    * (totalHeight - 1) / (config->Radius_Bottom + config->Radius_Top);
        int32_t corrected_radius_bottom = (float)config->Radius_Bottom * (totalHeight - 1) / (config->Radius_Bottom + config->Radius_Top);
        config->Radius_Top    = corrected_radius_top;
        config->Radius_Bottom = corrected_radius_bottom;
      }

      int32_t TLc_y = centerY + config->OffsetY - config->Height/2 + config->Radius_Top    - delta_y_top;
      int32_t TLc_x = centerX + config->OffsetX - config->Width/2  + config->Radius_Top;
      int32_t TRc_y = centerY + config->OffsetY - config->Height/2 + config->Radius_Top    + delta_y_top;
      int32_t TRc_x = centerX + config->OffsetX + config->Width/2  - config->Radius_Top;
      int32_t BLc_y = centerY + config->OffsetY + config->Height/2 - config->Radius_Bottom - delta_y_bottom;
      int32_t BLc_x = centerX + config->OffsetX - config->Width/2  + config->Radius_Bottom;
      int32_t BRc_y = centerY + config->OffsetY + config->Height/2 - config->Radius_Bottom + delta_y_bottom;
            int32_t BRc_x = centerX + config->OffsetX + config->Width/2  - config->Radius_Bottom;

      // 主体矩形
      FillRectangle(
        centerX + config->OffsetX - config->Width/2 + config->Radius_Top,
        centerY + config->OffsetY - config->Height/2 - delta_y_top,
        centerX + config->OffsetX + config->Width/2 - config->Radius_Top,
        centerY + config->OffsetY + config->Height/2 + delta_y_bottom,
        config->Color
      );

      // 左右侧矩形
      FillRectangle(
        centerX + config->OffsetX - config->Width/2,
        TLc_y, BLc_x, BLc_y,
        config->Color
      );
      FillRectangle(
        TRc_x, TRc_y,
        centerX + config->OffsetX + config->Width/2,
        BRc_y,
        config->Color
      );

      // 四个圆角
      FillEllipseCorner(TLc_x, TLc_y, config->Radius_Top,    config->Radius_Top,    1, 1, config->Color);
      FillEllipseCorner(TRc_x, TRc_y, config->Radius_Top,    config->Radius_Top,    0, 1, config->Color);
      FillEllipseCorner(BLc_x, BLc_y, config->Radius_Bottom, config->Radius_Bottom, 1, 0, config->Color);
      FillEllipseCorner(BRc_x, BRc_y, config->Radius_Bottom, config->Radius_Bottom, 0, 0, config->Color);

      // 斜角三角形
      FillRectangularTriangle(
        centerX + config->OffsetX - config->Width/2 + config->Radius_Top,
        centerY + config->OffsetY - config->Height/2 - delta_y_top,
        delta_y_top + delta_y_bottom,
        config->Width - config->Radius_Top - config->Radius_Bottom,
        true, config->Color
      );
      FillRectangularTriangle(
        centerX + config->OffsetX - config->Width/2 + config->Radius_Bottom,
        centerY + config->OffsetY + config->Height/2 - delta_y_bottom,
        delta_y_top + delta_y_bottom,
        config->Width - config->Radius_Top - config->Radius_Bottom,
        false, config->Color
      );
    }

  private:
    // u8g2.setDrawColor + u8g2.drawBox → eyeSprite.fillRect
    static void FillRectangle(int16_t x0, int16_t y0,
                               int16_t x1, int16_t y1,
                               uint8_t color) {
      uint32_t c = color ? TFT_WHITE : TFT_BLACK;
      int16_t x = min(x0, x1);
      int16_t y = min(y0, y1);
      int16_t w = abs(x1 - x0);
      int16_t h = abs(y1 - y0);
      if (w > 0 && h > 0) eyeSprite.fillRect(x, y, w, h, c);
    }

    // u8g2 ellipse corner → eyeSprite.drawPixel（精确逐像素填充）
    static void FillEllipseCorner(int16_t cx, int16_t cy,
                                   int16_t rx, int16_t ry,
                                   bool left, bool top,
                                   uint8_t color) {
      if (rx <= 0 || ry <= 0) return;
      uint32_t c = color ? TFT_WHITE : TFT_BLACK;
      for (int16_t y = 0; y <= ry; y++) {
        int16_t x = (int16_t)(rx * sqrt(1.0 - (float)(y*y) / (float)(ry*ry)));
        int16_t px = left  ? cx - x : cx + x - 1;
        int16_t py = top   ? cy - y : cy + y - 1;
        if (left) {
          eyeSprite.drawFastHLine(cx - x, py, x, c);
        } else {
          eyeSprite.drawFastHLine(cx,     py, x, c);
        }
        (void)px; // 消除未使用警告
      }
    }

    // u8g2.drawTriangle → eyeSprite 三角形填充
    static void FillRectangularTriangle(int16_t x, int16_t y,
                                         int16_t height, int16_t width,
                                         bool topLeft,
                                         uint8_t color) {
      if (height <= 0 || width <= 0) return;
      uint32_t c = color ? TFT_WHITE : TFT_BLACK;
      for (int16_t i = 0; i <= height; i++) {
        int16_t lineWidth = (int16_t)((float)width * i / height);
        if (topLeft) {
          eyeSprite.drawFastHLine(x, y + i, lineWidth, c);
        } else {
          eyeSprite.drawFastHLine(x, y - i, lineWidth, c);
        }
      }
    }
};


#endif

// ===== FaceEmotions.hpp =====
#ifndef _FACEEMOTIONS_h
#define _FACEEMOTIONS_h

#include <Arduino.h>

enum eEmotions {
	Normal=0,
	Angry,
	Glee,
	Happy,
	Sad,
	Worried,
	Focused,
	Annoyed,
	Surprised,
	Skeptic,
	Frustrated,
	Unimpressed,
	Sleepy,
	Suspicious,
	Squint,
	Furious,
	Scared,
	Awe,
	EMOTIONS_COUNT
};

#endif

// ===== EyePresets.h =====
/**
 * EyePresets.h
 * Defines the eye config associated with each emotion
 */

#ifndef _EYEPRESETS_h
#define _EYEPRESETS_h

#include <Arduino.h>

static const EyeConfig Preset_Normal = {
	.OffsetX = 0,
	.OffsetY = 0,
	.Height = 40,
	.Width = 40,
	.Slope_Top = 0,
	.Slope_Bottom = 0,
	.Radius_Top = 8,
	.Radius_Bottom = 8,
	.Inverse_Radius_Top = 0,
	.Inverse_Radius_Bottom = 0,
	.Inverse_Offset_Top = 0,
	.Inverse_Offset_Bottom = 0
};

static const EyeConfig Preset_Happy = {
	.OffsetX = 0,
	.OffsetY = 0,
	.Height = 10,
	.Width = 40,
	.Slope_Top = 0,
	.Slope_Bottom = 0,
	.Radius_Top = 10,
	.Radius_Bottom = 0,
	.Inverse_Radius_Top = 0,
	.Inverse_Radius_Bottom = 0,
	.Inverse_Offset_Top = 0,
	.Inverse_Offset_Bottom = 0
};

static const EyeConfig Preset_Glee = {
	.OffsetX = 0,
	.OffsetY = 0,
	.Height = 8,
	.Width = 40,
	.Slope_Top = 0,
	.Slope_Bottom = 0,
	.Radius_Top = 8,
	.Radius_Bottom = 0,
	.Inverse_Radius_Top = 0,
	.Inverse_Radius_Bottom = 5,
	.Inverse_Offset_Top = 0,
	.Inverse_Offset_Bottom = 0
};

static const EyeConfig Preset_Sad = {
	.OffsetX = 0,
	.OffsetY = 0,
	.Height = 15,
	.Width = 40,
	.Slope_Top = -0.5,
	.Slope_Bottom = 0,
	.Radius_Top = 1,
	.Radius_Bottom = 10,
	.Inverse_Radius_Top = 0,
	.Inverse_Radius_Bottom = 0,
	.Inverse_Offset_Top = 0,
	.Inverse_Offset_Bottom = 0
};

static const EyeConfig Preset_Worried = {
	.OffsetX = 0,
	.OffsetY = 0,
	.Height = 25,
	.Width = 40,
	.Slope_Top = -0.1,
	.Slope_Bottom = 0,
	.Radius_Top = 6,
	.Radius_Bottom = 10,
	.Inverse_Radius_Top = 0,
	.Inverse_Radius_Bottom = 0,
	.Inverse_Offset_Top = 0,
	.Inverse_Offset_Bottom = 0
};

static const EyeConfig Preset_Worried_Alt = {
	.OffsetX = 0,
	.OffsetY = 0,
	.Height = 35,
	.Width = 40,
	.Slope_Top = -0.2,
	.Slope_Bottom = 0,
	.Radius_Top = 6,
	.Radius_Bottom = 10,
	.Inverse_Radius_Top = 0,
	.Inverse_Radius_Bottom = 0,
	.Inverse_Offset_Top = 0,
	.Inverse_Offset_Bottom = 0
};

static const EyeConfig Preset_Focused = {
	.OffsetX = 0,
	.OffsetY = 0,
	.Height = 14,
	.Width = 40,
	.Slope_Top = 0.2,
	.Slope_Bottom = 0,
	.Radius_Top = 3,
	.Radius_Bottom = 1,
	.Inverse_Radius_Top = 0,
	.Inverse_Radius_Bottom = 0,
	.Inverse_Offset_Top = 0,
	.Inverse_Offset_Bottom = 0
};

static const EyeConfig Preset_Annoyed = {
	.OffsetX = 0,
	.OffsetY = 0,
	.Height = 12,
	.Width = 40,
	.Slope_Top = 0,
	.Slope_Bottom = 0,
	.Radius_Top = 0,
	.Radius_Bottom = 10,
	.Inverse_Radius_Top = 0,
	.Inverse_Radius_Bottom = 0,
	.Inverse_Offset_Top = 0,
	.Inverse_Offset_Bottom = 0
};

static const EyeConfig Preset_Annoyed_Alt = {
	.OffsetX = 0,
	.OffsetY = 0,
	.Height = 5,
	.Width = 40,
	.Slope_Top = 0,
	.Slope_Bottom = 0,
	.Radius_Top = 0,
	.Radius_Bottom = 4,
	.Inverse_Radius_Top = 0,
	.Inverse_Radius_Bottom = 0,
	.Inverse_Offset_Top = 0,
	.Inverse_Offset_Bottom = 0
};

static const EyeConfig Preset_Surprised = {
	.OffsetX = -2,
	.OffsetY = 0,
	.Height = 45,
	.Width = 45,
	.Slope_Top = 0,
	.Slope_Bottom = 0,
	.Radius_Top = 16,
	.Radius_Bottom = 16,
	.Inverse_Radius_Top = 0,
	.Inverse_Radius_Bottom = 0,
	.Inverse_Offset_Top = 0,
	.Inverse_Offset_Bottom = 0
};

static const EyeConfig Preset_Skeptic = {
	.OffsetX = 0,
	.OffsetY = 0,
	.Height = 40,
	.Width = 40,
	.Slope_Top = 0,
	.Slope_Bottom = 0,
	.Radius_Top = 10,
	.Radius_Bottom = 10,
	.Inverse_Radius_Top = 0,
	.Inverse_Radius_Bottom = 0,
	.Inverse_Offset_Top = 0,
	.Inverse_Offset_Bottom = 0
};

static const EyeConfig Preset_Skeptic_Alt = {
	.OffsetX = 0,
	.OffsetY = -6,
	.Height = 26,
	.Width = 40,
	.Slope_Top = 0.3,
	.Slope_Bottom = 0,
	.Radius_Top = 1,
	.Radius_Bottom = 10,
	.Inverse_Radius_Top = 0,
	.Inverse_Radius_Bottom = 0,
	.Inverse_Offset_Top = 0,
	.Inverse_Offset_Bottom = 0
};

static const EyeConfig Preset_Frustrated = {
	.OffsetX = 3,
	.OffsetY = -5,
	.Height = 12,
	.Width = 40,
	.Slope_Top = 0,
	.Slope_Bottom = 0,
	.Radius_Top = 0,
	.Radius_Bottom = 10,
	.Inverse_Radius_Top = 0,
	.Inverse_Radius_Bottom = 0,
	.Inverse_Offset_Top = 0,
	.Inverse_Offset_Bottom = 0
};

static const EyeConfig Preset_Unimpressed = {
	.OffsetX = 3,
	.OffsetY = 0,
	.Height = 12,
	.Width = 40,
	.Slope_Top = 0,
	.Slope_Bottom = 0,
	.Radius_Top = 1,
	.Radius_Bottom = 10,
	.Inverse_Radius_Top = 0,
	.Inverse_Radius_Bottom = 0,
	.Inverse_Offset_Top = 0,
	.Inverse_Offset_Bottom = 0
};

static const EyeConfig Preset_Unimpressed_Alt = {
	.OffsetX = 3,
	.OffsetY = -3,
	.Height = 22,
	.Width = 40,
	.Slope_Top = 0,
	.Slope_Bottom = 0,
	.Radius_Top = 1,
	.Radius_Bottom = 16,
	.Inverse_Radius_Top = 0,
	.Inverse_Radius_Bottom = 0,
	.Inverse_Offset_Top = 0,
	.Inverse_Offset_Bottom = 0
};

static const EyeConfig Preset_Sleepy = {
	.OffsetX = 0,
	.OffsetY = -2,
	.Height = 14,
	.Width = 40,
	.Slope_Top = -0.5,
	.Slope_Bottom = -0.5,
	.Radius_Top = 3,
	.Radius_Bottom = 3,
	.Inverse_Radius_Top = 0,
	.Inverse_Radius_Bottom = 0,
	.Inverse_Offset_Top = 0,
	.Inverse_Offset_Bottom = 0
};

static const EyeConfig Preset_Sleepy_Alt = {
	.OffsetX = 0,
	.OffsetY = -2,
	.Height = 8,
	.Width = 40,
	.Slope_Top = -0.5,
	.Slope_Bottom = -0.5,
	.Radius_Top = 3,
	.Radius_Bottom = 3,
	.Inverse_Radius_Top = 0,
	.Inverse_Radius_Bottom = 0,
	.Inverse_Offset_Top = 0,
	.Inverse_Offset_Bottom = 0
};

static const EyeConfig Preset_Suspicious = {
	.OffsetX = 0,
	.OffsetY = 0,
	.Height = 22,
	.Width = 40,
	.Slope_Top = 0,
	.Slope_Bottom = 0,
	.Radius_Top = 8,
	.Radius_Bottom = 3,
	.Inverse_Radius_Top = 0,
	.Inverse_Radius_Bottom = 0,
	.Inverse_Offset_Top = 0,
	.Inverse_Offset_Bottom = 0
};

static const EyeConfig Preset_Suspicious_Alt = {
	.OffsetX = 0,
	.OffsetY = -3,
	.Height = 16,
	.Width = 40,
	.Slope_Top = 0.2,
	.Slope_Bottom = 0,
	.Radius_Top = 6,
	.Radius_Bottom = 3,
	.Inverse_Radius_Top = 0,
	.Inverse_Radius_Bottom = 0,
	.Inverse_Offset_Top = 0,
	.Inverse_Offset_Bottom = 0
};

static const EyeConfig Preset_Squint = {
	.OffsetX = -10,
	.OffsetY = -3,
	.Height = 35,
	.Width = 35,
	.Slope_Top = 0,
	.Slope_Bottom = 0,
	.Radius_Top = 8,
	.Radius_Bottom = 8,
	.Inverse_Radius_Top = 0,
	.Inverse_Radius_Bottom = 0,
	.Inverse_Offset_Top = 0,
	.Inverse_Offset_Bottom = 0
};

static const EyeConfig Preset_Squint_Alt = {
	.OffsetX = 5,
	.OffsetY = 0,
	.Height = 20,
	.Width = 20,
	.Slope_Top = 0,
	.Slope_Bottom = 0,
	.Radius_Top = 5,
	.Radius_Bottom = 5,
	.Inverse_Radius_Top = 0,
	.Inverse_Radius_Bottom = 0,
	.Inverse_Offset_Top = 0,
	.Inverse_Offset_Bottom = 0
};

static const EyeConfig Preset_Angry = {
	.OffsetX = -3,
	.OffsetY = 0,
	.Height = 20,
	.Width = 40,
	.Slope_Top = 0.3,
	.Slope_Bottom = 0,
	.Radius_Top = 2,
	.Radius_Bottom = 12,
	.Inverse_Radius_Top = 0,
	.Inverse_Radius_Bottom = 0,
	.Inverse_Offset_Top = 0,
	.Inverse_Offset_Bottom = 0
};

static const EyeConfig Preset_Furious = {
	.OffsetX = -2,
	.OffsetY = 0,
	.Height = 30,
	.Width = 40,
	.Slope_Top = 0.4,
	.Slope_Bottom = 0,
	.Radius_Top = 2,
	.Radius_Bottom = 8,
	.Inverse_Radius_Top = 0,
	.Inverse_Radius_Bottom = 0,
	.Inverse_Offset_Top = 0,
	.Inverse_Offset_Bottom = 0
};

static const EyeConfig Preset_Scared = {
	.OffsetX = -3,
	.OffsetY = 0,
	.Height = 40,
	.Width = 40,
	.Slope_Top = -0.1,
	.Slope_Bottom = 0,
	.Radius_Top = 12,
	.Radius_Bottom = 8,
	.Inverse_Radius_Top = 0,
	.Inverse_Radius_Bottom = 0,
	.Inverse_Offset_Top = 0,
	.Inverse_Offset_Bottom = 0
};

static const EyeConfig Preset_Awe = {
	.OffsetX = 2,
	.OffsetY = 0,
	.Height = 35,
	.Width = 45,
	.Slope_Top = -0.1,
	.Slope_Bottom = 0.1,
	.Radius_Top = 12,
	.Radius_Bottom = 12,
	.Inverse_Radius_Top = 0,
	.Inverse_Radius_Bottom = 0,
	.Inverse_Offset_Top = 0,
	.Inverse_Offset_Bottom = 0
};

#endif

// ===== BlinkAssistant.h =====
#ifndef _BLINKASSISTANT_h
#define _BLINKASSISTANT_h

#include <Arduino.h>

class Face;

class BlinkAssistant {
 protected:
	Face&  _face;

 public:
	BlinkAssistant(Face& face);

	AsyncTimer Timer;

	void Update();
	void Blink();
};

#endif

// ===== LookAssistant.h =====
#ifndef _LOOKASSISTANT_h
#define _LOOKASSISTANT_h

#include <Arduino.h>

class Face;

class LookAssistant
{
 protected:
	Face&  _face;

 public:
	LookAssistant(Face& face);

	Transformation transformation;

	AsyncTimer Timer;

	void LookAt(float x, float y);
	void Update();
};

#endif

// ===== Eye.h =====
#ifndef _EYE_h
#define _EYE_h

#include <Arduino.h>

class Face;

class Eye {
  protected:
    Face& _face;

    void Update();
    void ChainOperators();

  public:
    Eye(Face& face);

    uint16_t CenterX;
    uint16_t CenterY;
    bool IsMirrored = false;

    EyeConfig Config;
    EyeConfig* FinalConfig;

    EyeTransition Transition;
    EyeTransformation Transformation;
    EyeVariation Variation1;
    EyeVariation Variation2;
    EyeBlink BlinkTransformation;

    void ApplyPreset(const EyeConfig preset);
    void TransitionTo(const EyeConfig preset);
    void Draw();
};

#endif

// ===== FaceExpression.h =====
#ifndef _FACEEXPRESSION_h
#define _FACEEXPRESSION_h

#include <Arduino.h>

class Face;

class FaceExpression {
  protected:
    Face&  _face;

  public:
    FaceExpression(Face& face);

    void ClearVariations();

    void GoTo_Normal();
    void GoTo_Angry();
    void GoTo_Glee();
    void GoTo_Happy();
    void GoTo_Sad();
    void GoTo_Worried();
    void GoTo_Focused();
    void GoTo_Annoyed();
    void GoTo_Surprised();
    void GoTo_Skeptic();
    void GoTo_Frustrated();
    void GoTo_Unimpressed();
    void GoTo_Sleepy();
    void GoTo_Suspicious();
    void GoTo_Squint();
    void GoTo_Furious();
    void GoTo_Scared();
    void GoTo_Awe();
};

#endif

// ===== FaceBehavior.h =====
#ifndef _FACEBEHAVIOR_h
#define _FACEBEHAVIOR_h

#include <Arduino.h>

class Face;

class FaceBehavior
{
 protected:
	Face&  _face;

 public:
	FaceBehavior(Face& face);

	eEmotions CurrentEmotion;

	float Emotions[eEmotions::EMOTIONS_COUNT];

	AsyncTimer Timer;

	void SetEmotion(eEmotions emotion, float value);
	float GetEmotion(eEmotions emotion);

	void Clear();
	void Update();
	eEmotions GetRandomEmotion();

	void GoToEmotion(eEmotions emotion);
};

#endif

// ===== Face.h =====
#ifndef _FACE_h
#define _FACE_h

#include <Arduino.h>

class Face {

public:
    Face(uint16_t screenWidth, uint16_t screenHeight, uint16_t eyeSize);

    uint16_t Width;
    uint16_t Height;
    uint16_t CenterX;
    uint16_t CenterY;
    uint16_t EyeSize;
    uint16_t EyeInterDistance = 4;

    Eye LeftEye;
    Eye RightEye;
    BlinkAssistant Blink;
    LookAssistant Look;
    FaceBehavior Behavior;
    FaceExpression Expression;

    void Update();
    void DoBlink();

    bool RandomBehavior = true;
    bool RandomLook = true;
    bool RandomBlink = true;

    void LookLeft();
    void LookRight();
    void LookFront();
    void LookTop();
    void LookBottom();
    void Wait(unsigned long milliseconds);

protected:
    void Draw();
};

#endif