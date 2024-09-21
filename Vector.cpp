#include <cmath>
#include <iostream>
#include "Vectors.hpp"
#include <vector>
#include <fstream>

Vector::Vector(){
    this->x = 0;
    this->y = 0;
    this->z = 0;
}
Vector::Vector(double x, double y, double z){
    this->x = x;
    this->y = y;
    this->z = z;
}

double Vector::dot_product(Vector vec) const{
    double ctr_1 = vec.x * this->x;
    double ctr_2 = vec.y * this->y;
    double ctr_3 = vec.z * this->z;
    double sum = ctr_1 + ctr_2 + ctr_3;
    return sum;
}

double Vector::magnitude() const{
    double ctr_1 = this->x * this->x;
    double ctr_2 = this->y * this->y;
    double ctr_3 = this->z * this->z;
    double sum = ctr_1 + ctr_2 + ctr_3;
    sum = sqrt(sum);
    return sum;
}


Vector Vector::project(Vector vec) const{
    double vec_dot = vec.dot_product(vec);
    double this_dot = this->dot_product(vec);
    Vector div = (this_dot / vec_dot) * vec;
    return div;
}

Vector Vector::operator-(Vector vec) const{
    double x = this->x - vec.x;
    double y = this->y - vec.y;
    double z = this->z - vec.z;
    Vector new_vec = Vector(x, y, z);
    return new_vec;
}

Vector Vector::operator-() const{
    double x = -this->x;
    double y = -this->y;
    double z = -this->z;
    Vector new_vec = Vector(x, y, z);
    return new_vec;
}

Vector Vector::operator+(Vector vec) const{
    double x = this->x + vec.x;
    double y = this->y + vec.y;
    double z = this->z + vec.z;
    Vector new_vec = Vector(x, y, z);
    return new_vec;
}

Vector operator*(double input, Vector vec){
    double x = input * vec.x;
    double y = input * vec.y;
    double z = input * vec.z;
    return Vector(x, y, z);
}


Ray::Ray(const Vector start, const Vector dir){
    this->start = start;
    this->dir_vec = dir;
}

Vector Ray::get_start() const{
    return this->start;
}

Vector Ray::get_dir() const{
    return this->dir_vec;
}

Color::Color(){
    this->red = 0;
    this->green = 0;
    this->blue = 0;
}
double Color::get_red(){
    return this->red;
}

double Color::get_green(){
    return this->green;
}

double Color::get_blue(){
    return this->blue;
}

Color::Color(double red, double green, double blue){
    this->red = red;
    this->green = green;
    this->blue = blue;
}

Color Color::operator+(Color color) const{
    double red = this->red + color.red;
    double green = this->green + color.green;
    double blue = this->blue + color.blue;
    Color new_col = Color(red, green, blue);
    return new_col;
}

void Color::operator+=(Color color){
    this->red = this->red + color.red;
    this->green = this->green + color.green;
    this->blue = this->blue + color.blue;
}

Color operator*(double val, Color color){
    double red = color.red * val;
    double green = color.green * val;
    double blue = color.blue * val;
    return Color(red, green, blue);
}

Shape::Shape(Color color, double reflect){
    this->color = color;
    this->reflect = reflect;
}

Color Shape::get_color() const{
    return this->color;
}

double Shape::get_reflectivity() const{
    return this->reflect;
}

Sphere::Sphere(Color color, double reflect, Vector center_point, double radius) : Shape(color, reflect){
    this->center_point = center_point;
    this->radius = radius;
}

Vector Sphere::get_normal_vector(Vector vec){
    return vec - this->center_point;
}

double Sphere::get_collision_time(Ray ray){
    double a = ray.get_dir().dot_product(ray.get_dir());
    Vector bb = 2 * ray.get_dir();
    double b = bb.dot_product(this->get_normal_vector(ray.get_start()));
    double c = this->get_normal_vector(ray.get_start()).dot_product(this->get_normal_vector(ray.get_start())) -  
                (this->radius * this->radius);
    double check = (b*b) - (4*a*c);
    if (check < 0){
        return -1;
    }double t1 = (-b + sqrt(check)) / (2 * a);
    double t2 = (-b - sqrt(check)) / (2 * a);
    if (t1 < 0 && t2 < 0){
        return -1;
    }if (t1 < 0 && t2 >= 0){
        return t2;
    }if (t2 < 0 && t1 >= 0){
        return t1;
    }
    return std::min(t1, t2);
}

Plane::Plane(Color col, double reflect, Vector point, Vector norm_vec) : Shape(col, reflect){
    this->point = point;
    this->norm_vec = norm_vec;
}

// Plane::~Plane(){
//     delete this->point;
//     delete this->norm_vec;
// }

// Sphere::~Sphere(){
//     delete this->center_point;
// }

double Plane::get_collision_time(Ray ray){
    double check = ray.get_dir().dot_product(this->norm_vec);
    if (std::abs(check) < 0.000001){
        return -1;
    }
    double compute1 = (this->point - ray.get_start()).dot_product(this->norm_vec);
    return compute1 / check;
}

Vector Plane::get_normal_vector(Vector vec [[maybe_unused]]){ // put in Vector vec ?
    return this->norm_vec;
}


Scene::Scene(Vector cam, Vector lit, Color back = Color(135, 206, 235), double amb = 0.2, double fac = 0.5, int exp = 8, int max = 6){
    this->cam_pos = cam;
    this->lit_pos = lit;
    this->back_color = back;
    this->array_shapes = new Shape*[100];
    this->size = 0;
    this->ambient = amb;
    this->spec_fac = fac;
    this->spec_exp = exp;
    this->max_ref = max;

}

Color Scene::get_point_color(Vector point){
    Vector ray_dir = point - this->cam_pos;
    Ray ray(this->cam_pos, ray_dir);

    return get_ray_color(ray, this->max_ref);
}


Color Scene::get_ray_color(const Ray& ray, double max_ref){
    double min_t = std::numeric_limits<double>::infinity();
    Shape* closest_shape = nullptr;

    for (int i = 0; i < this->size; i++) {
        double t = this->array_shapes[i]->get_collision_time(ray);
        if (t >= 0 && t < min_t) {
            min_t = t;
            closest_shape = this->array_shapes[i];
        }
    }

    if (closest_shape == nullptr) {
        return this->back_color;
    }

    Vector intersection_point = ray.get_start() + (min_t * ray.get_dir());

    Vector normal_vector = closest_shape->get_normal_vector(intersection_point);
    normal_vector = (1 / normal_vector.magnitude()) * normal_vector;


    Color ambient_color = (this->ambient * (1 - closest_shape->get_reflectivity())) * closest_shape->get_color();

    Vector light_direction = (this->lit_pos - intersection_point);
    double light_intensity = light_direction.magnitude();
    light_direction = (1 / light_intensity) * light_direction;
    double diffuse_factor = std::max(0.0, normal_vector.dot_product(light_direction));
    Color diffuse_color = (1 - this->ambient) * (1 - closest_shape->get_reflectivity()) * diffuse_factor * closest_shape->get_color();


    Vector h = (light_direction + (1 / ray.get_dir().magnitude()) * (-ray.get_dir()));
    h =  0.5 * (1 / h.magnitude()) * h;
    double specular_factor = std::pow(std::max(0.0, h.dot_product(normal_vector)), this->spec_exp);
    Color specular_color = this->spec_fac * specular_factor * Color(255, 255, 255); 


    Color reflected_color;
    if (max_ref > 0) {
        Vector norm = (1 / ray.get_dir().magnitude()) * ray.get_dir();
        Vector reflected_dir = (-norm + 2 * (-norm - (norm.project(normal_vector))));
        Ray reflected_ray(intersection_point + (1e-6 * reflected_dir), reflected_dir);
        reflected_color = (1 - this->ambient) * closest_shape->get_reflectivity() * get_ray_color(reflected_ray, max_ref-1);
    } else {
        reflected_color = Color();
    }

    return ambient_color + diffuse_color + specular_color + reflected_color;
}


void write_ppm(const Array2D<Color>& pixels, const std::string& filename) {
  std::ofstream out(filename, std::ios::binary | std::ios::out);
  out.write("P6\n", 3);
  std::string s = std::to_string(pixels.get_width());
  out.write(s.c_str(), s.length());
  out.write(" ", 1);
  s = std::to_string(pixels.get_height());
  out.write(s.c_str(), s.length());
  out.write("\n255\n", 5);
  char color[3];
  for (unsigned y = 0; y < pixels.get_height(); y++) {
    for (unsigned x = 0; x < pixels.get_width(); x++) {
      Color c = pixels.get(x, y);
      color[0] = c.get_red();
      color[1] = c.get_green();
      color[2] = c.get_blue();
      out.write(color, 3);
    }
  }
  out.close();
}


// Scene::Scene(Vector cam, Vector lit){
//     this->cam_pos = cam;
//     this->lit_pos = lit;
//     this->back_color = Color(135, 206, 235);
//     this->array_shapes = new Shape*[100];
//     this->size = 0;
// }

void Scene::add_shape(Shape* shape){
    this->array_shapes[this->size] = shape;
    this->size++;
}

Scene::~Scene(){
    for (int i = 0; i < this->size; i++){
        delete this->array_shapes[i];
    }delete[] this->array_shapes;
}


// Vector Shape::get_normal_vector(Vector vec) = 0;

// double Shape::get_collision_time(Ray ray) = 0;

int main(){
    // Sphere* sp1 = new Sphere(Color(255, 0, 0), 0.3, Vector(2, 0, 0), 0.4);
    // Ray ray(Vector(0,0,0), Vector(1,0,0));
    // std::cout<< sp1->get_collision_time(ray) << std::endl;

    // Plane* plane = new Plane(Color(0, 0, 0), 0, Vector(0,0,0), Vector(0, 0, 1));
    // Ray ray(Vector(0,0,2), Vector(0,0,-1));
    // std::cout<< plane->get_collision_time(ray) << std::endl;

    Vector camera_position(0.5, -1.0, 0.5); 
    Vector light_position(0.0, -0.5, 1.0);
    // Color background_color(0, 0, 0); 
    Scene scene(camera_position, light_position);

    unsigned image_width = 512; 
    unsigned image_height = 512; 
    Array2D<Color> pixels(image_width, image_height);

    Plane* plane = new Plane(Color(255, 255, 255), 0, Vector(0,0,0), Vector(0, 0, 1));
    Sphere* sp1 = new Sphere(Color(255, 0, 0), 0.3, Vector(0.25, 0.45, 0.4), 0.4);
    Sphere* sp2 = new Sphere(Color(0, 255, 0), 0, Vector(1, 1, 0.25), 0.25);
    Sphere* sp3 = new Sphere(Color(0, 0, 255), 0.7, Vector(0.8, 0.3, 0.15), 0.15);
    scene.add_shape(plane);
    scene.add_shape(sp1);
    scene.add_shape(sp2);
    scene.add_shape(sp3);

    for (unsigned y = 0; y < image_height; y++) {
        for (unsigned x = 0; x < image_width; x++) {
            Vector screen_point(x / ((double)image_width), 0, (image_height - y) / ((double)image_width));
            Color pixel_color = scene.get_point_color(screen_point);
            pixels.set(x, y, pixel_color);
        }
    }
    
    write_ppm(pixels, "output_image.ppm");
    //Vector fabian(3,4,5);
    // Vector* fabian = new Vector(3,4,5);
    // Vector* frank = new Vector(6,7,8);
    // Vector res = -(*fabian);
    // // std::cout << res << std::endl;
    return 0;
}