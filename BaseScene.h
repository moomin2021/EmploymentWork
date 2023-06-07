#pragma once

class BaseScene {
public:
	BaseScene() {};
	virtual ~BaseScene() {};
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
};