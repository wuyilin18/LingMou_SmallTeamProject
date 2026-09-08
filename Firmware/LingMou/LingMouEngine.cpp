// lib/LingMouEngine/LingMouEngine.cpp
#include "LingMouEngine.h"
#include <TFT_eSPI.h>
// ===== AsyncTimer.cpp =====
AsyncTimer::AsyncTimer(unsigned long millisInterval) : AsyncTimer(millisInterval, nullptr) {}

AsyncTimer::AsyncTimer(unsigned long millisInterval, AsyncTimerCallback onFinish) {
	Interval = millisInterval;
	OnFinish = onFinish;
}

void AsyncTimer::Start() {
	Reset();
	_isActive = true;
}

void AsyncTimer::Reset() {
	_startTime = millis();
}

void AsyncTimer::Stop() {
	_isActive = false;
}

bool AsyncTimer::Update() {
	if (_isActive == false) return false;

	_isExpired = false;
	if (static_cast<unsigned long>(millis() - _startTime) >= Interval) {
		_isExpired = true;
		if (OnFinish != nullptr) OnFinish();
		Reset();
	}
	return _isExpired;
}

void AsyncTimer::SetIntervalMillis(unsigned long interval) {
	Interval = interval;
}

unsigned long AsyncTimer::GetStartTime() {
	return _startTime;
}

unsigned long AsyncTimer::GetElapsedTime() {
	return millis() - _startTime;
}

unsigned long AsyncTimer::GetRemainingTime() {
	return Interval - millis() + _startTime;
}

bool AsyncTimer::IsActive() const {
	return _isActive;
}

bool AsyncTimer::IsExpired() const {
	return _isExpired;
}

// ===== EyeTransition.cpp =====
EyeTransition::EyeTransition() : Animation(500) {}

void EyeTransition::Update() {
	float t = Animation.GetValue();
	Apply(t);
}

void EyeTransition::Apply(float t) {
	Origin->OffsetX = Origin->OffsetX * (1.0 - t) + Destin.OffsetX * t;
	Origin->OffsetY = Origin->OffsetY * (1.0 - t) + Destin.OffsetY * t;
	Origin->Height = Origin->Height * (1.0 - t) + Destin.Height * t;
	Origin->Width = Origin->Width * (1.0 - t) + Destin.Width * t;
	Origin->Slope_Top = Origin->Slope_Top * (1.0 - t) + Destin.Slope_Top * t;
	Origin->Slope_Bottom = Origin->Slope_Bottom * (1.0 - t) + Destin.Slope_Bottom * t;
	Origin->Radius_Top = Origin->Radius_Top * (1.0 - t) + Destin.Radius_Top * t;
	Origin->Radius_Bottom = Origin->Radius_Bottom * (1.0 - t) + Destin.Radius_Bottom * t;
	Origin->Inverse_Radius_Top = Origin->Inverse_Radius_Top * (1.0 - t) + Destin.Inverse_Radius_Top * t;
	Origin->Inverse_Radius_Bottom = Origin->Inverse_Radius_Bottom * (1.0 - t) + Destin.Inverse_Radius_Bottom * t;
	Origin->Inverse_Offset_Top = Origin->Inverse_Offset_Top * (1.0 - t) + Destin.Inverse_Offset_Top * t;
	Origin->Inverse_Offset_Bottom = Origin->Inverse_Offset_Bottom * (1.0 - t) + Destin.Inverse_Offset_Bottom * t;
}

// ===== EyeTransformation.cpp =====
EyeTransformation::EyeTransformation() : Animation(200)
{
}

void EyeTransformation::Update()
{
	auto t = Animation.GetValue();
	Current.MoveX = (Destin.MoveX - Origin.MoveX) * t + Origin.MoveX;
	Current.MoveY = (Destin.MoveY - Origin.MoveY) * t + Origin.MoveY;
	Current.ScaleX = (Destin.ScaleX - Origin.ScaleX) * t + Origin.ScaleX;
	Current.ScaleY = (Destin.ScaleY - Origin.ScaleY) * t + Origin.ScaleY;

	Apply();
}

void EyeTransformation::Apply()
{
	Output.OffsetX = Input->OffsetX + Current.MoveX;
	Output.OffsetY = Input->OffsetY - Current.MoveY;
	Output.Width = Input->Width * Current.ScaleX;
	Output.Height = Input->Height * Current.ScaleY;

	Output.Slope_Top = Input->Slope_Top;
	Output.Slope_Bottom = Input->Slope_Bottom;
	Output.Radius_Top = Input->Radius_Top;
	Output.Radius_Bottom = Input->Radius_Bottom;
	Output.Inverse_Radius_Top = Input->Inverse_Radius_Top;
	Output.Inverse_Radius_Bottom = Input->Inverse_Radius_Bottom;
	Output.Inverse_Offset_Top = Input->Inverse_Offset_Top;
	Output.Inverse_Offset_Bottom = Input->Inverse_Offset_Bottom;
}

void EyeTransformation::SetDestin(Transformation transformation)
{
	Origin.MoveX = Current.MoveX;
	Origin.MoveY = Current.MoveY;
	Origin.ScaleX = Current.ScaleX;
	Origin.ScaleY = Current.ScaleY;

	Destin.MoveX = transformation.MoveX;
	Destin.MoveY = transformation.MoveY;
	Destin.ScaleX = transformation.ScaleX;
	Destin.ScaleY = transformation.ScaleY;
}

// ===== EyeVariation.cpp =====
EyeVariation::EyeVariation() : Animation(0, 1000, 0, 1000, 0) {}

void EyeVariation::Clear() {
	Values.OffsetX = 0;
	Values.OffsetY = 0;
	Values.Height = 0;
	Values.Width = 0;
	Values.Slope_Top = 0;
	Values.Slope_Bottom = 0;
	Values.Radius_Top = 0;
	Values.Radius_Bottom = 0;
	Values.Inverse_Radius_Top = 0;
	Values.Inverse_Radius_Bottom = 0;
	Values.Inverse_Offset_Top = 0;
	Values.Inverse_Offset_Bottom = 0;
}

void EyeVariation::Update() {
	auto t = Animation.GetValue();
	Apply(2.0 * t - 1.0);
}

void EyeVariation::Apply(float t) {
	Output.OffsetX = Input->OffsetX + Values.OffsetX * t;
	Output.OffsetY = Input->OffsetY + Values.OffsetY * t;
	Output.Height = Input->Height + Values.Height * t;;
	Output.Width = Input->Width + Values.Width * t;
	Output.Slope_Top = Input->Slope_Top + Values.Slope_Top * t;
	Output.Slope_Bottom = Input->Slope_Bottom + Values.Slope_Bottom * t;
	Output.Radius_Top = Input->Radius_Top + Values.Radius_Top * t;
	Output.Radius_Bottom = Input->Radius_Bottom + Values.Radius_Bottom * t;
	Output.Inverse_Radius_Top = Input->Inverse_Radius_Top + Values.Inverse_Radius_Top * t;
	Output.Inverse_Radius_Bottom = Input->Inverse_Radius_Bottom + Values.Inverse_Radius_Bottom * t;
	Output.Inverse_Offset_Top = Input->Inverse_Offset_Top + Values.Inverse_Offset_Top * t;
	Output.Inverse_Offset_Bottom = Input->Inverse_Offset_Bottom + Values.Inverse_Offset_Bottom * t;;
}

// ===== EyeBlink.cpp =====
EyeBlink::EyeBlink() : Animation(40, 100, 40) {}

void EyeBlink::Update() {
	auto t = Animation.GetValue();
	if (Animation.GetElapsed() > Animation.Interval) t = 0.0;
	Apply(t * t);
}


void EyeBlink::Apply(float t) {
	Output.OffsetX = Input->OffsetX;
	Output.OffsetY = Input->OffsetY;

	Output.Width = (BlinkWidth - Input->Width) * t + Input->Width;
	Output.Height = (BlinkHeight - Input->Height) * t + Input->Height;

	Output.Slope_Top = Input->Slope_Top * (1.0 - t);
	Output.Slope_Bottom = Input->Slope_Bottom * (1.0 - t);
	Output.Radius_Top = Input->Radius_Top * (1.0 - t);
	Output.Radius_Bottom = Input->Radius_Bottom * (1.0 - t);
	Output.Inverse_Radius_Top = Input->Inverse_Radius_Top * (1.0 - t);
	Output.Inverse_Radius_Bottom = Input->Inverse_Radius_Bottom * (1.0 - t);
	Output.Inverse_Offset_Top = Input->Inverse_Offset_Top * (1.0 - t);
	Output.Inverse_Offset_Bottom = Input->Inverse_Offset_Bottom * (1.0 - t);
}

// ===== BlinkAssistant.cpp =====
BlinkAssistant::BlinkAssistant(Face& face) : _face(face), Timer(3500) {
	Timer.Start();
}

void BlinkAssistant::Update() {
	Timer.Update();

	if (Timer.IsExpired()) {
		Blink();
	}
}

void BlinkAssistant::Blink() {
	_face.LeftEye.BlinkTransformation.Animation.Restart();
	_face.RightEye.BlinkTransformation.Animation.Restart();
	Timer.Reset();
}

// ===== LookAssistant.cpp =====
LookAssistant::LookAssistant(Face& face) : _face(face), Timer(4000)
{
	Timer.Start();
}

void LookAssistant::LookAt(float x, float y)
{
	int16_t moveX_x;
	int16_t moveY_x;
	int16_t moveY_y;
	float scaleY_x;
	float scaleY_y;

	// What is this witchcraft...?!
	moveX_x = -30 * x;
	moveY_x = -3 * x;
	moveY_y = 22 * y;
	scaleY_x = 1.0 - x * 0.2;
	scaleY_y = 1.0 - (y > 0 ? y : -y) * 0.4;

	transformation.MoveX = moveX_x;
	transformation.MoveY = moveY_y; //moveY_x + moveY_y;
	transformation.ScaleX = 1.0;
	transformation.ScaleY = scaleY_x * scaleY_y;
	_face.RightEye.Transformation.SetDestin(transformation);

	moveY_x = +3 * x;
	scaleY_x = 1.0 + x * 0.2;
	transformation.MoveX = moveX_x;
	transformation.MoveY = +moveY_y; //moveY_x + moveY_y;
	transformation.ScaleX = 1.0;
	transformation.ScaleY = scaleY_x * scaleY_y;
	_face.LeftEye.Transformation.SetDestin(transformation);

	_face.RightEye.Transformation.Animation.Restart();
	_face.LeftEye.Transformation.Animation.Restart();
}

void LookAssistant::Update() {
	Timer.Update();

	if (Timer.IsExpired()) {
		Timer.Reset();
		auto x = random(-50, 50);
		auto y = random(-50, 50);
		LookAt((float)x / 100, (float)y / 100);
	}

}

// ===== Eye.cpp =====
Eye::Eye(Face& face) : _face(face) {

	this->IsMirrored = false;

	ChainOperators();
	Variation1.Animation._t0 = 200;
	Variation1.Animation._t1 = 200;
	Variation1.Animation._t2 = 200;
	Variation1.Animation._t3 = 200;
	Variation1.Animation._t4 = 0;
	Variation1.Animation.Interval = 800;

	Variation2.Animation._t0 = 0;
	Variation2.Animation._t1 = 200;
	Variation2.Animation._t2 = 200;
	Variation2.Animation._t3 = 200;
	Variation2.Animation._t4 = 200;
	Variation2.Animation.Interval = 800;
}

void Eye::ChainOperators() {
	Transition.Origin = &Config;
	Transformation.Input = &Config;
	Variation1.Input = &(Transformation.Output);
	Variation2.Input = &(Variation1.Output);
	BlinkTransformation.Input = &(Variation2.Output);
	FinalConfig = &(BlinkTransformation.Output);
}

void Eye::Update() {
	Transition.Update();
	Transformation.Update();
	Variation1.Update();
	Variation2.Update();
	BlinkTransformation.Update();
}

void Eye::Draw() {
	Update();

	// 1.5x visual scale for the landscape eye mode.
	// FinalConfig already contains expression, variation, blink and LookAt offsets,
	// so scaling here enlarges both the eyes and their visible movement range.
	const float EYE_VISUAL_SCALE = 1.5f;
	EyeConfig scaled = *FinalConfig;

	scaled.OffsetX = (int16_t)roundf(FinalConfig->OffsetX * EYE_VISUAL_SCALE);
	scaled.OffsetY = (int16_t)roundf(FinalConfig->OffsetY * EYE_VISUAL_SCALE);
	scaled.Height = (int16_t)roundf(FinalConfig->Height * EYE_VISUAL_SCALE);
	scaled.Width = (int16_t)roundf(FinalConfig->Width * EYE_VISUAL_SCALE);

	scaled.Radius_Top = (int16_t)roundf(FinalConfig->Radius_Top * EYE_VISUAL_SCALE);
	scaled.Radius_Bottom = (int16_t)roundf(FinalConfig->Radius_Bottom * EYE_VISUAL_SCALE);
	scaled.Inverse_Radius_Top = (int16_t)roundf(FinalConfig->Inverse_Radius_Top * EYE_VISUAL_SCALE);
	scaled.Inverse_Radius_Bottom = (int16_t)roundf(FinalConfig->Inverse_Radius_Bottom * EYE_VISUAL_SCALE);
	scaled.Inverse_Offset_Top = (int16_t)roundf(FinalConfig->Inverse_Offset_Top * EYE_VISUAL_SCALE);
	scaled.Inverse_Offset_Bottom = (int16_t)roundf(FinalConfig->Inverse_Offset_Bottom * EYE_VISUAL_SCALE);

	// Slopes and color are dimensionless / unchanged.
	EyeDrawer::Draw(CenterX, CenterY, &scaled);
}

void Eye::ApplyPreset(const EyeConfig config) {
	Config.OffsetX = this->IsMirrored ? -config.OffsetX : config.OffsetX;
	Config.OffsetY = -config.OffsetY;
	Config.Height = config.Height;
	Config.Width = config.Width;
	Config.Slope_Top = this->IsMirrored ? config.Slope_Top : -config.Slope_Top;
	Config.Slope_Bottom = this->IsMirrored ? config.Slope_Bottom : -config.Slope_Bottom;
	Config.Radius_Top = config.Radius_Top;
	Config.Radius_Bottom = config.Radius_Bottom;
	Config.Inverse_Radius_Top = config.Inverse_Radius_Top;
	Config.Inverse_Radius_Bottom = config.Inverse_Radius_Bottom;

	// ★ 补上
	Config.Inverse_Offset_Top = config.Inverse_Offset_Top;
	Config.Inverse_Offset_Bottom = config.Inverse_Offset_Bottom;
	Config.Color = config.Color;

	// ★ 让初始目标也有确定值
	Transition.Destin = Config;

	Transition.Animation.Restart();
}

void Eye::TransitionTo(const EyeConfig config) {
	Transition.Destin.OffsetX =
		this->IsMirrored ? -config.OffsetX : config.OffsetX;

	Transition.Destin.OffsetY = -config.OffsetY;
	Transition.Destin.Height = config.Height;
	Transition.Destin.Width = config.Width;

	Transition.Destin.Slope_Top =
		this->IsMirrored ? config.Slope_Top : -config.Slope_Top;

	Transition.Destin.Slope_Bottom =
		this->IsMirrored ? config.Slope_Bottom : -config.Slope_Bottom;

	Transition.Destin.Radius_Top = config.Radius_Top;
	Transition.Destin.Radius_Bottom = config.Radius_Bottom;

	Transition.Destin.Inverse_Radius_Top =
		config.Inverse_Radius_Top;

	Transition.Destin.Inverse_Radius_Bottom =
		config.Inverse_Radius_Bottom;

	// ★ 原来漏掉的
	Transition.Destin.Inverse_Offset_Top =
		config.Inverse_Offset_Top;

	Transition.Destin.Inverse_Offset_Bottom =
		config.Inverse_Offset_Bottom;

	Transition.Destin.Color = config.Color;

	Transition.Animation.Restart();
}


// ===== FaceExpression.cpp =====
FaceExpression::FaceExpression(Face& face) : _face(face)
{
}

void FaceExpression::ClearVariations()
{
	_face.RightEye.Variation1.Clear();
	_face.RightEye.Variation2.Clear();
	_face.LeftEye.Variation1.Clear();
	_face.LeftEye.Variation2.Clear();
	_face.RightEye.Variation1.Animation.Restart();
	_face.LeftEye.Variation1.Animation.Restart();
}

void FaceExpression::GoTo_Normal()
{
	ClearVariations();

	_face.RightEye.Variation1.Values.Height = 3;
	_face.RightEye.Variation2.Values.Width = 1;
	_face.LeftEye.Variation1.Values.Height = 2;
	_face.LeftEye.Variation2.Values.Width = 2;
	_face.RightEye.Variation1.Animation.SetTriangle(1000, 0);
	_face.LeftEye.Variation1.Animation.SetTriangle(1000, 0);

	_face.RightEye.TransitionTo(Preset_Normal);
	_face.LeftEye.TransitionTo(Preset_Normal);

}

void FaceExpression::GoTo_Angry()
{
	ClearVariations();
	_face.RightEye.Variation1.Values.OffsetY = 2;
	_face.LeftEye.Variation1.Values.OffsetY = 2;
	_face.RightEye.Variation1.Animation.SetTriangle(300, 0);
	_face.LeftEye.Variation1.Animation.SetTriangle(300, 0);

	_face.RightEye.TransitionTo(Preset_Angry);
	_face.LeftEye.TransitionTo(Preset_Angry);
}

void FaceExpression::GoTo_Glee()
{
	ClearVariations();
	_face.RightEye.Variation1.Values.OffsetY = 5;
	_face.LeftEye.Variation1.Values.OffsetY = 5;
	_face.RightEye.Variation1.Animation.SetTriangle(300, 0);
	_face.LeftEye.Variation1.Animation.SetTriangle(300, 0);

	_face.RightEye.TransitionTo(Preset_Glee);
	_face.LeftEye.TransitionTo(Preset_Glee);
}

void FaceExpression::GoTo_Happy()
{
	ClearVariations();
	_face.RightEye.TransitionTo(Preset_Happy);
	_face.LeftEye.TransitionTo(Preset_Happy);
}

void FaceExpression::GoTo_Sad()
{
	ClearVariations();
	_face.RightEye.TransitionTo(Preset_Sad);
	_face.LeftEye.TransitionTo(Preset_Sad);
}

void FaceExpression::GoTo_Worried()
{
	ClearVariations();
	_face.RightEye.TransitionTo(Preset_Worried);
	_face.LeftEye.TransitionTo(Preset_Worried_Alt);
}

void FaceExpression::GoTo_Focused()
{
	ClearVariations();
	_face.RightEye.TransitionTo(Preset_Focused);
	_face.LeftEye.TransitionTo(Preset_Focused);
}

void FaceExpression::GoTo_Annoyed()
{
	ClearVariations();
	_face.RightEye.TransitionTo(Preset_Annoyed);
	_face.LeftEye.TransitionTo(Preset_Annoyed_Alt);
}

void FaceExpression::GoTo_Surprised()
{
	ClearVariations();
	_face.RightEye.TransitionTo(Preset_Surprised);
	_face.LeftEye.TransitionTo(Preset_Surprised);
}

void FaceExpression::GoTo_Skeptic()
{
	ClearVariations();
	_face.RightEye.TransitionTo(Preset_Skeptic);
	_face.LeftEye.TransitionTo(Preset_Skeptic_Alt);
}

void FaceExpression::GoTo_Frustrated()
{
	ClearVariations();
	_face.RightEye.TransitionTo(Preset_Frustrated);
	_face.LeftEye.TransitionTo(Preset_Frustrated);
}

void FaceExpression::GoTo_Unimpressed()
{
	ClearVariations();
	_face.RightEye.TransitionTo(Preset_Unimpressed);
	_face.LeftEye.TransitionTo(Preset_Unimpressed_Alt);
}

void FaceExpression::GoTo_Sleepy()
{
	ClearVariations();
	_face.RightEye.TransitionTo(Preset_Sleepy);
	_face.LeftEye.TransitionTo(Preset_Sleepy_Alt);
}

void FaceExpression::GoTo_Suspicious()
{
	ClearVariations();
	_face.RightEye.TransitionTo(Preset_Suspicious);
	_face.LeftEye.TransitionTo(Preset_Suspicious_Alt);
}

void FaceExpression::GoTo_Squint()
{
	ClearVariations();

	_face.LeftEye.Variation1.Values.OffsetX = 6;
	_face.LeftEye.Variation2.Values.OffsetY = 6;

	_face.RightEye.TransitionTo(Preset_Squint);
	_face.LeftEye.TransitionTo(Preset_Squint_Alt);

}

void FaceExpression::GoTo_Furious()
{
	ClearVariations();

	_face.RightEye.TransitionTo(Preset_Furious);
	_face.LeftEye.TransitionTo(Preset_Furious);
}

void FaceExpression::GoTo_Scared()
{
	ClearVariations();

	_face.RightEye.TransitionTo(Preset_Scared);
	_face.LeftEye.TransitionTo(Preset_Scared);
}

void FaceExpression::GoTo_Awe()
{
	ClearVariations();

	_face.RightEye.TransitionTo(Preset_Awe);
	_face.LeftEye.TransitionTo(Preset_Awe);
}

// ===== FaceBehavior.cpp =====
FaceBehavior::FaceBehavior(Face& face) : _face(face), Timer(4000) {
	// 🔴 FIX: 把原来的 Timer(500) 改成 4000！500毫秒变一次脸太像抽风了，4秒变一次最优雅！
	Timer.Start();
	Clear();

	// 🔴 FIX: 把所有 18 个表情的“被抽中概率”全部设置为 1.0！
	// 否则它永远只能抽中 Normal！
	for (int emotion = 0; emotion < eEmotions::EMOTIONS_COUNT; emotion++) {
		Emotions[emotion] = 1.0;
	}
}

void FaceBehavior::SetEmotion(eEmotions emotion, float value) {
	Emotions[emotion] = value;
}

float FaceBehavior::GetEmotion(eEmotions emotion) {
	return Emotions[emotion];
}

void FaceBehavior::Clear() {
	for (int emotion = 0; emotion < eEmotions::EMOTIONS_COUNT; emotion++) {
		Emotions[emotion] = 0.0;
	}
}

// Use roulette wheel to select a new emotion, based on assigned weights
eEmotions FaceBehavior::GetRandomEmotion() {

	// Calculate the total sum of all emotional weights
	float sum_of_weight = 0;
	for (int emotion = 0; emotion < eEmotions::EMOTIONS_COUNT; emotion++) {
		sum_of_weight += Emotions[emotion];
	}
	// If no weights have been assigned, default to "normal" emotion
	if (sum_of_weight == 0) {
		return eEmotions::Normal;
	}
	// Now pick a random number that lies somewhere in the range of total weights
	float rand = random(0, 1000 * sum_of_weight) / 1000.0;
	// Loop over emotions and select the one whose probabity distribution contains
	// the value in which the random number lies
	float acc = 0;
	for (int emotion = 0; emotion < eEmotions::EMOTIONS_COUNT; emotion++) {
		if (Emotions[emotion] == 0) continue;
		acc += Emotions[emotion];
		if (rand <= acc) {
			return (eEmotions)emotion;
		}
	}
	// If something goes wrong in the calculation, return "normal"
	return eEmotions::Normal;
}

void FaceBehavior::Update() {
	Timer.Update();

	if (Timer.IsExpired()) {
		Timer.Reset();
		eEmotions newEmotion = GetRandomEmotion();
		if (CurrentEmotion != newEmotion) {
			GoToEmotion(newEmotion);
		}
	}
}

void FaceBehavior::GoToEmotion(eEmotions emotion) {
	// Set the currentEmotion to the desired emotion
	CurrentEmotion = emotion;

	// Call the appropriate expression transition function 
	switch (CurrentEmotion) {
	case eEmotions::Normal: _face.Expression.GoTo_Normal(); break;
	case eEmotions::Angry: _face.Expression.GoTo_Angry(); break;
	case eEmotions::Glee: _face.Expression.GoTo_Glee(); break;
	case eEmotions::Happy: _face.Expression.GoTo_Happy(); break;
	case eEmotions::Sad: _face.Expression.GoTo_Sad(); break;
	case eEmotions::Worried: _face.Expression.GoTo_Worried(); break;
	case eEmotions::Focused: _face.Expression.GoTo_Focused(); break;
	case eEmotions::Annoyed: _face.Expression.GoTo_Annoyed(); break;
	case eEmotions::Surprised: _face.Expression.GoTo_Surprised(); break;
	case eEmotions::Skeptic: _face.Expression.GoTo_Skeptic(); break;
	case eEmotions::Frustrated: _face.Expression.GoTo_Frustrated(); break;
	case eEmotions::Unimpressed: _face.Expression.GoTo_Unimpressed(); break;
	case eEmotions::Sleepy: _face.Expression.GoTo_Sleepy(); break;
	case eEmotions::Suspicious: _face.Expression.GoTo_Suspicious(); break;
	case eEmotions::Squint: _face.Expression.GoTo_Squint(); break;
	case eEmotions::Furious: _face.Expression.GoTo_Furious(); break;
	case eEmotions::Scared: _face.Expression.GoTo_Scared(); break;
	case eEmotions::Awe: _face.Expression.GoTo_Awe(); break;
	default: break;
	}
}

// ===== Face.cpp =====
Face::Face(uint16_t screenWidth, uint16_t screenHeight, uint16_t eyeSize)
	: LeftEye(*this), RightEye(*this), Blink(*this), Look(*this), Behavior(*this), Expression(*this) {

	// Unlike almost every other Arduino library (and the I2C address scanner script etc.)
	// u8g2 uses 8-bit I2C address, so we shift the 7-bit address left by one

	Width = screenWidth;
	Height = screenHeight;
	EyeSize = eyeSize;

	CenterX = Width / 2;
	CenterY = Height / 2;

	LeftEye.IsMirrored = true;


	Behavior.Timer.Start();
}

void Face::LookFront() {
	Look.LookAt(0.0, 0.0);
}

void Face::LookRight() {
	Look.LookAt(-1.0, 0.0);
}

void Face::LookLeft() {
	Look.LookAt(1.0, 0.0);
}

void Face::LookTop() {
	Look.LookAt(0.0, 1.0);
}

void Face::LookBottom() {
	Look.LookAt(0.0, -1.0);
}

void Face::Wait(unsigned long milliseconds) {
	unsigned long start;
	start = millis();
	while (millis() - start < milliseconds) {
		Draw();
	}
}

void Face::DoBlink() {
	Blink.Blink();
}

void Face::Update() {
	if (RandomBehavior) Behavior.Update();
	if (RandomLook) Look.Update();
	if (RandomBlink)	Blink.Update();
	Draw();
}

void Face::Draw() {
	eyeSprite.fillSprite(TFT_BLACK);

	LeftEye.CenterX = CenterX - EyeSize / 2 - EyeInterDistance;
	LeftEye.CenterY = CenterY;

	RightEye.CenterX = CenterX + EyeSize / 2 + EyeInterDistance;
	RightEye.CenterY = CenterY;

	LeftEye.Draw();
	RightEye.Draw();
}
