#include "Common.h"
#include <iostream>

using namespace std;

// Этот файл сдаётся на проверку
// Здесь напишите реализацию необходимых классов-потомков `IShape`


class Shape : public IShape {
public:
	void SetPosition(Point position) override {
		position_ = position;
	}
	Point GetPosition() const override {
		return position_;
	}

	void SetSize(Size size) override {
		size_ = size;
	}
	Size GetSize() const override {
		return size_;
	}

	void SetTexture(shared_ptr<ITexture> texture) override {
		texture_ = move(texture);
	}
	ITexture* GetTexture() const override {
		return texture_.get();
	}

	void Draw(Image& image) const override {
		Image texture;
		if (texture_) {
			texture = texture_->GetImage();
		}
		Size image_size = { 
			static_cast<int>(image[0].size()), 
			static_cast<int>(image.size()) 
		};
		for (int y = 0; y < size_.height; ++y) {
			for (int x = 0; x < size_.width; ++x) {
				if (GetType() == ShapeType::Ellipse && 
					!IsPointInEllipse({ x, y }, GetSize())) {
					continue;
				}
				char unit = '.';
				if (texture_ && x < texture[0].size() && y < texture.size()) {
					unit = texture[y][x];
				}
				if (position_.x + x < image_size.width && 
					position_.y + y < image_size.height) {
					image[position_.y + y][position_.x + x] = unit;
				}
			}
		}
	}

	virtual ShapeType GetType() const = 0;

private:
	shared_ptr<ITexture> texture_;
	Point position_ = {};
	Size size_ = {};
};


class Rectangle : public Shape {
public:
	unique_ptr<IShape> Clone() const override {
		return make_unique<Rectangle>(*this);
	}

	ShapeType GetType() const override {
		return ShapeType::Rectangle;
	}	
};

class Ellipse : public Shape {
public:
	unique_ptr<IShape> Clone() const override {
		return make_unique<Ellipse>(*this);
	}

	ShapeType GetType() const override {
		return ShapeType::Ellipse;
	}	
};


unique_ptr<IShape> MakeShape(ShapeType shape_type) {
	switch (shape_type) {
	case ShapeType::Rectangle:
		return make_unique<Rectangle>();
	case ShapeType::Ellipse:
		return make_unique<Ellipse>();
	default:
		return nullptr;
	}
}