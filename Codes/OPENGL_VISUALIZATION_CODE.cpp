#include<iostream>
#include<string>
#include<fstream>
#include <stdlib.h>
#include<vector>
#include<sstream>
#include<algorithm>
#include <fstream>
#include<map>
#include <stdio.h>

#define GLEW_STATIC
#include<GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>

#include<glm/glm.hpp>
#include<glm/vec3.hpp>
#include<glm/vec4.hpp>
#include<glm/mat4x4.hpp>
#include<glm/ext.hpp>
#include<glm/gtx/string_cast.hpp>
GLFWwindow* window;

using namespace std;
#define ESC 27;

GLfloat light_diffuse[] = { 0.005, 0.005, 0.005, 1 };
GLfloat light_position[] = { 100.0, -50,0 , 0.5 }; 

vector<glm::vec3> vertices;
vector<glm::vec3> vertex_normals;
vector<glm::vec3> vertices2;
vector<glm::vec3> vertices3;
vector<glm::vec3> vertices4;
vector<glm::vec3> vertices5;
float sphi = 90.0, stheta = 45.0;

float xcam = 0, ycam = 0;
long xsize, ysize;
int downX, downY;
bool leftButton = false;

GLfloat camerax = -2.0, cameray = 2, cameraz = 2;

// below 5 functions are used for 4th stage in improving delaunay triangulation.
// Using the normals obtained from opend3d tool, we remove noise edges.

int vertexequal(glm::vec3 a, glm::vec3 b) {
    if (a.x == b.x && a.y == b.y && a.z == b.z)return 1;
    else return 0;
}

glm::vec3 findvector(glm::vec3 a, glm::vec3 b) {
    glm::vec3 temp;
    temp.x = a.x - b.x;
    temp.y = a.y - b.y;
    temp.z = a.z - b.z;
    //cout << temp.x << " " << temp.y << " " << temp.z << endl;
    float sum = temp.x * temp.x + temp.y * temp.y + temp.z * temp.z;
    sum = sqrt(sum);
    //cout << sum << endl;
    temp.x /= sum;
    temp.y /= sum;
    temp.z /= sum;
    return temp;
}
glm::vec3 findavgvector(vector<glm::vec3>a) {
    glm::vec3 temp = a[0];
    for (int i = 1;i < a.size();i++) {
        temp.x += a[i].x;
        temp.y += a[i].y;
        temp.z += a[i].z;
    }
    return temp;
}

int angle(glm::vec3 a, glm::vec3 b) {
    
    float dotproduct = a.x * b.x + a.y * b.y + a.z * b.z;
    float maga = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
    float magb = sqrt(b.x * b.x + b.y * b.y + b.z * b.z);
    float angle = std::acos(dotproduct/(maga *magb));
    angle = angle * 180.0 / 3.14159;
    
   
    if (angle > 135 || angle<45)return 0;
    else return 1;
}

vector<pair<glm::vec3, glm::vec3> >flaggededges;

void buildedges() {
    int n = vertices2.size();
    cout << n << endl;
    int z = n / 2;
    for (int i = 0;i < n;i++) {
        vector<glm::vec3>edges_list;
        for (int j = 0;j < z;j++) {
           
            glm::vec3 temp1 = vertices2[j*2];
            glm::vec3 temp2 = vertices2[j * 2+1];
           
            if (vertexequal(temp1, vertices2[i]) && !vertexequal(temp2, vertices2[i])) {
                edges_list.push_back(temp2);
            }
            else if(vertexequal(temp2, vertices2[i]) && !vertexequal(temp1, vertices2[i])) {
                edges_list.push_back(temp1);
            }
        }
        /*
        cout << "///////////////////////////////////" << endl;
        cout << "vertex is:" << endl;
        cout << vertices2[i].x << " " << vertices2[i].y << " " << vertices2[i].z << endl;
        cout << "edges are:" << endl;
        for (int j = 0;j < edges_list.size();j++) {
            cout << edges_list[j].x << " " << edges_list[j].y << " " << edges_list[j].z << endl;
        }
        */
        vector<glm::vec3>vectors_list;
        for (int j = 0;j < edges_list.size();j++) {
            vectors_list.push_back(findvector(vertices2[i], edges_list[j]));
        }
        glm::vec3 averagevector = vertex_normals[i];

        vector<glm::vec3>correct_vertices;
        for (int j = 0;j < edges_list.size();j++) {
            if (!angle(averagevector, vectors_list[j])) {
                flaggededges.push_back(make_pair(vertices2[i], edges_list[j]));
                flaggededges.push_back(make_pair(edges_list[j] ,vertices2[i] ));
            }
        }

    }
    
    z = vertices2.size() / 2;
    for (int i = 0;i < z;i++) {
        glm::vec3 v1 = vertices2[i * 2];
        glm::vec3 v2 = vertices2[i * 2 + 1];

        int flag = 0;
        for (int j = 0;j < flaggededges.size();j++) {
            
            if (vertexequal(flaggededges[j].first, v1) && vertexequal(flaggededges[j].second, v2)) { flag = 1;break; }
        }
        if (flag == 1)continue;
        
        vertices3.push_back(v1);
        vertices3.push_back(v2);
    }
    cout << vertices3.size() << endl;
  

}


// below 3 functions are used for rotation and zoom in 3d space
void MyKeyboardFunc(unsigned char Key, int x, int y)
{
    switch (Key)
    {
    case 27:
        cout << "Esc is called" << endl;
        exit(0);
        break;
    }
    glutPostRedisplay();
}

GLdouble zoom = -247.5f;
void MouseCallback(int button, int state, int x, int y) {
    GLdouble min_z = -249.0;
    GLdouble max_z = -220.0;

    if (button == 4 && zoom < max_z) {
        zoom += 0.25;
        cout << "Increase Zoom:"<<zoom<< endl;
    }
    else if (button == 3 && zoom > min_z) {
        zoom -= 0.25;
        cout << "Decrease Zoom:"<<zoom << endl;
    }

    downX = x; downY = y;
    leftButton = ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN));
  
    glutPostRedisplay();
}


void MotionCallback(int x, int y) {
    if (leftButton) { sphi += (float)(x - downX) / 4.0;stheta += (float)(downY - y) / 4.0; } // rotate
    downX = x;   downY = y;
    glutPostRedisplay();
}


// Load object is used to load diff types of files and store the vertices in diff vectors.

bool loadObject(const char* path) {

    vector<glm::vec3> temp_vertices;

    FILE* file = NULL;
    fopen_s(&file, path, "r");
    if (file == NULL) {
        cout << "Couldn't read the file" << endl;
        return false;
    }
    while (1) {
        if (path == "output_new_normals.txt") {
            glm::vec3 temp1;
            glm::vec3 normal1;
            int count = fscanf_s(file, "%f %f %f %f %f %f\n", &temp1.x, &temp1.y, &temp1.z, &normal1.x, &normal1.y, &normal1.z );
            if (count != 6)break;
            temp1.x *= 4;
            temp1.y *= 4;
            temp1.z *= 4;
            temp1.y -= 0.5;
            vertices2.push_back(temp1);
            vertex_normals.push_back(normal1);
        } 
        else if (path == "output_new.txt") {
            glm::vec3 temp1;
            int count = fscanf_s(file, "%f %f %f\n", &temp1.x, &temp1.y, &temp1.z);
            if (count != 3)break;
            temp1.x *= 4;
            temp1.y *= 4;
            temp1.z *= 4;
            temp1.y -= 1;
            vertices2.push_back(temp1);
        }
        else if (path == "output_15l_own.txt") {
            glm::vec3 temp1;
            int count = fscanf_s(file, "%f %f %f\n", &temp1.x, &temp1.y, &temp1.z);
            if (count != 3)break;
            temp1.x *= 1;
            temp1.y *= 1;
            temp1.z *= 1;
            temp1.y -= 1;
            vertices5.push_back(temp1);
        }
        else if (path == "output_new_clustering20.txt") {
            glm::vec3 temp1;
            int count = fscanf_s(file, "%f %f %f\n", &temp1.x, &temp1.y, &temp1.z);
            if (count != 3)break;
            temp1.x *= 4;
            temp1.y *= 4;
            temp1.z *= 4;
            temp1.y -= 1;
            vertices4.push_back(temp1);
        }
        else if (path == "dataset.txt") {
            glm::vec3 temp1;
            int output;
            int count = fscanf_s(file, "%f %f %f %f\n", &temp1.x, &temp1.y, &temp1.z, &output);
            if (count != 4)break;
            temp1.x *= 4;
            temp1.y *= 4;
            temp1.z *= 4;
            temp1.y -= 0.5;
            if (output >= 0)vertices3.push_back(temp1);
        }
        else {

            char lineHeader[256];
            int res = fscanf_s(file, "%s", lineHeader, 256);
            if (res == EOF) {
                break;
            }
            if (strcmp(lineHeader, "v") == 0) {
                glm::vec3 temp1;
                fscanf_s(file, "%f %f %f\n", &temp1.x, &temp1.y, &temp1.z);
                temp1.x *= 4;
                temp1.y *= 4;
                temp1.z *= 4;
                temp1.y -= 0.5;
                temp_vertices.push_back(temp1);
            }
            else if (strcmp(lineHeader, "f") == 0) {
                int vertex1, vertex2, vertex3;
                int count = fscanf_s(file, "%d %d %d\n", &vertex1, &vertex2, &vertex3);
                if (count != 3) {
                    cout << "ERROR" << endl;
                    return false;
                }
                glm::vec3 temp = temp_vertices[vertex1 - 1];
                vertices.push_back(temp);
                temp = temp_vertices[vertex2 - 1];
                vertices.push_back(temp);
                temp = temp_vertices[vertex3 - 1];
                vertices.push_back(temp);

            }
        }
    }
    return true;
}

// Below 4 functions are used to render diff types of models/ surfaces from the vertices
// stored in diff vectors.

void findNormal(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3& normal)
{
    glm::vec3 a1, b1;
    a1.x = a.x - b.x; a1.y = a.y - b.y; a1.z = a.z - b.z;
    b1.x = a.x - c.x; b1.y = a.y - c.y; b1.z = a.z - c.z;
    normal.x = a1.y * b1.z - b1.y * a1.z;
    normal.y = -1 * a1.x * b1.z + b1.x * a1.z;
    normal.z = a1.x * b1.y - b1.x * a1.y;
    float den = normal.x * normal.x + normal.y * normal.y + normal.z * normal.z;
    normal.x = normal.x / den;
    normal.y = normal.y / den;
    normal.z = normal.z / den;

}

void quad(vector<glm::vec3>vertices,int a, int b, int c, int d)
{
    cout << "yes";
    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.0f, 1.0f);
    float* data;
    data = glm::value_ptr(vertices[a]);
    glVertex3fv(data);
    data = glm::value_ptr(vertices[b]);
    glVertex3fv(data);
    data = glm::value_ptr(vertices[c]);
    glVertex3fv(data);
    data = glm::value_ptr(vertices[d]);
    glVertex3fv(data);
    glEnd();
}

void
drawcube(glm::vec3 vertex) {

    vector<glm::vec3>vertices;
    cout << vertex.x << " " << vertex.y << " " << vertex.z << endl;
    glm::vec3 temp;
    float a = 0.1;
    temp.x = vertex.x + 0; temp.y =vertex.y + 0; temp.z = vertex.z+ 0;
    vertices.push_back(vertex);
    temp.x = vertex.x + 0; temp.y = vertex.y + 0; temp.z = vertex.z + a;
    vertices.push_back(vertex);
    temp.x = vertex.x + 0; temp.y = vertex.y + a; temp.z = vertex.z + 0;
    vertices.push_back(vertex);
    temp.x = vertex.x + a; temp.y = vertex.y + 0; temp.z = vertex.z + 0;
    vertices.push_back(vertex);
    temp.x = vertex.x + 0; temp.y = vertex.y + a; temp.z = vertex.z + a;
    vertices.push_back(vertex);
    temp.x = vertex.x + a; temp.y = vertex.y + a; temp.z = vertex.z + 0;
    vertices.push_back(vertex);
    temp.x = vertex.x + a; temp.y = vertex.y + 0; temp.z = vertex.z + a;
    vertices.push_back(vertex);
    temp.x = vertex.x + a; temp.y = vertex.y + a; temp.z = vertex.z + a;
    vertices.push_back(vertex);

    quad(vertices, 0, 3, 2, 1);
    quad(vertices, 2, 3, 7, 6);
    quad(vertices, 0, 4, 7, 3);
    quad(vertices, 1, 2, 6, 5);
    quad(vertices, 4, 5, 6, 7);
    quad(vertices, 1, 0, 4, 5);
}

void
drawBox(void)
{
    /*
    int i;
    int x = vertices3.size();
    drawcube(vertices3[0]);
    
    for (i = 0;i < x;i++) {
        drawcube(vertices3[i]);
    }
    */
    int i;
    int n = vertices.size();
    cout << n << endl;
    int z = vertices3.size() / 2;
    cout << z << endl;
    glm::vec3 normal;
    for (i = 0; i < z; i++) {
        
        
        glBegin(GL_LINES);

        float dist = ( (vertices3[i * 2].x - vertices3[i * 2 + 1].x) * (vertices3[i * 2].x - vertices3[i * 2 + 1].x) +
            (vertices3[i * 2].y - vertices3[i * 2 + 1].y) * (vertices3[i * 2].y - vertices3[i * 2 + 1].y) +
            (vertices3[i * 2].z - vertices3[i * 2 + 1].z) * (vertices3[i * 2].z - vertices3[i * 2 + 1].z) );
        if (dist >0.5)continue;

        //cout << dist << endl;

        float* data;
        data = glm::value_ptr(vertices3[i * 2]);
        glVertex3fv(data);
        data = glm::value_ptr(vertices3[i * 2 + 1]);
        glVertex3fv(data);
        glEnd();
    }
    int zz = vertices2.size() / 2;
    zz = 0;
    for (i = 0; i < zz; i++) {


        glBegin(GL_LINES);

        float dist = ((vertices2[i * 2].x - vertices2[i * 2 + 1].x) * (vertices2[i * 2].x - vertices2[i * 2 + 1].x) +
            (vertices2[i * 2].y - vertices2[i * 2 + 1].y) * (vertices2[i * 2].y - vertices2[i * 2 + 1].y) +
            (vertices2[i * 2].z - vertices2[i * 2 + 1].z) * (vertices2[i * 2].z - vertices2[i * 2 + 1].z));
        //if (dist > 0.5)continue;

        //cout << dist << endl;

        float* data;
        data = glm::value_ptr(vertices2[i * 2]);
        glVertex3fv(data);
        data = glm::value_ptr(vertices2[i * 2 + 1]);
        glVertex3fv(data);
        glEnd();
    }

    int z2 = vertices5.size();

    for (i = 0; i < z2; i++) {
        glBegin(GL_LINES);
        glm::vec3 temp;
        temp.x = vertices5[i].x + 0.01;
        temp.y = vertices5[i].y;
        temp.z = vertices5[i].z;

        float* data;
        data = glm::value_ptr(vertices5[i]);
        glVertex3fv(data);
        data = glm::value_ptr(temp);
        glVertex3fv(data);
        glEnd();
    }

    int z1 = vertices4.size() / 2;
   
    for (i = 0; i < z1; i++) {


        glBegin(GL_LINES);

        float dist = ((vertices4[i * 2].x - vertices4[i * 2 + 1].x) * (vertices4[i * 2].x - vertices4[i * 2 + 1].x) +
            (vertices4[i * 2].y - vertices4[i * 2 + 1].y) * (vertices4[i * 2].y - vertices4[i * 2 + 1].y) +
            (vertices4[i * 2].z - vertices4[i * 2 + 1].z) * (vertices4[i * 2].z - vertices4[i * 2 + 1].z));
        if (dist > 0.5)continue;

        //cout << dist << endl;

        float* data;
        data = glm::value_ptr(vertices4[i * 2]);
        glVertex3fv(data);
        data = glm::value_ptr(vertices4[i * 2 + 1]);
        glVertex3fv(data);
        glEnd();
    }
    n = n / 3;
    for(i =0;i<n;i++){
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin(GL_TRIANGLES);
        findNormal(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2], normal);
        float* data = glm::value_ptr(normal);
        glNormal3fv(data);
        //cout << normal.x << " " << normal.y << " " << normal.z << endl;

        //glColor3f(0.53, 0.81, 0.94);
        data = glm::value_ptr(vertices[i * 3]);
        glVertex3fv(data);

        //glColor3f(0.53, 0.81, 0.94);
        data = glm::value_ptr(vertices[i * 3 + 1]);
        glVertex3fv(data);

        //glColor3f(0.53, 0.81, 0.94);
        data = glm::value_ptr(vertices[i * 3 + 2]);
        glVertex3fv(data);

        glEnd();
        
    }
    
    glBegin(GL_LINES);
    glVertex3f(0.0f, 1.5f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 1.5f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.5f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glEnd();
}

void
display(void)
{

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLdouble ortho = 250 + zoom;
    glOrtho(-ortho, ortho, -ortho, ortho, -250, 250);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(-stheta, 1.0, 0.0, 0.0);
    glRotatef(sphi, 0.0, 1.0, 0.0);
    
    drawBox();
    glutSwapBuffers();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void
init(void)
{

    /* Enable a single OpenGL light. */
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);                         
    glEnable(GL_DEPTH_TEST);

    /* Setup the view of the cube. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-250, 250, -250, 250, -250, 250);
    //gluPerspective(40.0, 1, 1.0, 10.0);

    glMatrixMode(GL_MODELVIEW);


    gluLookAt(camerax, cameray, cameraz,  /* eye is at (0,0,5) */
        0, 0, 0,      /* center is at (0,0,0) */
        0.0, 1.0, 0.);      /* up is in positive Y direction */

      /* Adjust cube position to be asthetic angle. */
    glTranslatef(0.0, -0.5, 0.0);
    glRotatef(-90, 0, 1, 0.0);
}


// This is the main function used for visualization. We load diff types of files into diff vectors
// and render them using the display function.
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("poly bunny");

    //loadObject("output_new_normals.txt");
    //loadObject("output_new_normals.txt");
    loadObject("output_15l_own.txt");
    //loadObject("output_new.txt");
    //loadObject("lowpolybunny.obj");
    //loadObject("output_new.txt");
    //int result2 = loadObject("lowpolybunny.obj");
    //cout << vertices.size() << endl;
    //cout << vertices2.size() << endl;
    //buildedges();
    //cout << vertices3.size() << endl;
    //cout << vertices2.size() << endl;
   

    
    ofstream myfile("output_final_edges.txt");
    for (int i = 0;i < vertices3.size();i++) {
        
        if (myfile.is_open())
        {
            myfile << vertices3[i].x;
            myfile << " ";
            myfile << vertices3[i].y;
            myfile << " ";
            myfile << vertices3[i].z;
            myfile << '\n';
        }
    }
    myfile.close();
    
    
    glutDisplayFunc(display);
    glutKeyboardFunc(MyKeyboardFunc);
    glutMouseFunc(MouseCallback);
    glutMotionFunc(MotionCallback);
    init();
    glutMainLoop();
    return 0;        
}
