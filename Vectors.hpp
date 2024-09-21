#include <vector>
#include <fstream>
#pragma once


class Vector {
    private:
        double x;
        double y;
        double z;


    public:
        Vector();
        Vector(double, double, double);

        double dot_product(Vector) const;
        double magnitude() const;
        Vector project(Vector) const;

        Vector operator-(Vector) const;
        Vector operator-() const;
        Vector operator+(Vector) const;

        friend Vector operator*(double, Vector);

};

class Ray{
    private:
        Vector start;
        Vector dir_vec;

    public:
        Ray(const Vector start, const Vector dir_vec);
        Vector get_start() const;
        Vector get_dir() const;
};

class Color{
    private:
        double red;
        double green;
        double blue;
    
    public:
        double get_red();
        double get_green();
        double get_blue();
        Color();
        Color(double red, double green, double blue);
        Color operator+(Color) const;
        void operator+=(Color);
        friend Color operator*(double, Color);
};


class Shape{
    private:
        Color color;
        double reflect;

    public:
        Shape(Color, double);
        Color get_color() const;
        double get_reflectivity() const;
    virtual ~Shape() {};
    virtual Vector get_normal_vector(Vector) = 0; 
    virtual double get_collision_time(Ray) = 0;

};

class Sphere : public Shape{
    private:
        Vector center_point;
        double radius;
    
    public:
        // ~Sphere();
        Sphere(Color, double, Vector, double);
        virtual Vector get_normal_vector(Vector); 
        virtual double get_collision_time(Ray);
};

class  Plane : public Shape{
    private:
        Vector point;
        Vector norm_vec;
    
    public:
        // ~Plane();
        Plane(Color, double, Vector, Vector);
        virtual Vector get_normal_vector(Vector); 
        virtual double get_collision_time(Ray);
};

class Scene{
    private: 
        Vector cam_pos;
        Vector lit_pos;
        Color back_color;
        Shape** array_shapes;
        int size;
        double ambient;
        double spec_fac;
        int spec_exp;
        int max_ref;

    
    public:
        Scene(Vector, Vector, Color, double, double, int, int );
        // Scene(Vector, Vector);
        void add_shape(Shape*);
        virtual ~Scene();
        Color get_point_color(Vector);
        Color get_ray_color(const Ray&, double);

};

template<typename T> class Array2D{
    private:
        unsigned width;
        unsigned height;
        std::vector<std::vector<T> > array;

    public:
        // Construct a new array with the given width and height.
        Array2D(unsigned width,  unsigned height){
            this->width = width;
            this->height = height;
            std::vector<std::vector<T>> ans;
            for (unsigned i = 0; i < width; i++){
                std::vector<T> a(height);
                ans.push_back(a);
            }
            this->array = ans;
        }

        // Delete an array
        ~Array2D(){};

        // Get the width of the array
        unsigned get_width() const{
            return this->width;
        }

        // Get the height of the array
        unsigned get_height() const{
            return this->height;
        }


        // Get an element from the array given horizontal and vertical position
        T get(unsigned x, unsigned y) const{
            return this->array[x][y];
        }

        // Set an element of the array given a horizontal and vertical position
        void set(unsigned x, unsigned y, T value){
            this->array[x][y] = value;
        }
        Array2D(const Array2D&) = delete;
    
};