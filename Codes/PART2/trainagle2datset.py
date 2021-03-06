import numpy as np
import random
import math


class Vec3:
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z

    def sub(self, v):
        return Vec3(self.x - v.x,
                    self.y - v.y,
                    self.z - v.z)

    def dot(self, v):
        return self.x * v.x + self.y * v.y + self.z * v.z

    def cross(self, v):
        return Vec3(self.y * v.z - self.z * v.y,
                    self.z * v.x - self.x * v.z,
                    self.x * v.y - self.y * v.x)

    def length(self):
        return math.sqrt(self.x * self.x +
                         self.y * self.y +
                         self.z * self.z)

    def normalize(self):
        l = self.length()
        return Vec3(self.x / l, self.y / l, self.z / l)


class Ray:
    def __init__(self, orig=None, direction=None):
        self.orig = orig
        self.direction = direction


def ray_triangle_intersect(r, v0, v1, v2):
    v0v1 = v1.sub(v0)
    v0v2 = v2.sub(v0)
    pvec = r.direction.cross(v0v2)

    det = v0v1.dot(pvec)

    if det < 0.000001:
        return float('-inf')

    invDet = 1.0 / det
    tvec = r.orig.sub(v0)
    u = tvec.dot(pvec) * invDet

    if u < 0 or u > 1:
        return float('-inf')

    qvec = tvec.cross(v0v1)
    v = r.direction.dot(qvec) * invDet

    if v < 0 or u + v > 1:
        return float('-inf')

    return v0v2.dot(qvec) * invDet


def find(O,D,A,B,C):
    # return 1
    # u = (O-A,E2,-D) / (E1,E2,-D)
    E1 = B-A
    E2 = C-A

    num = np.array([ O-A, E2, -D])
    den = np.array([E1, E2, -D])
    u = np.linalg.det(num) / np.linalg.det(den)
    u = 1
    
    # v = (E1,O-A,-D) / (E1 ,E2,-D)
    num = np.array([ E1, O-A, -D])
    den = np.array([E1, E2, -D])
    v = np.linalg.det(num) / np.linalg.det(den)
    v = 1

    # t = (E1,E2,O-A) / (E1,E2,-D)
    num = np.array([ E1, E2, O-A])
    den = np.array([E1, E2, -D])
    t = np.linalg.det(num) / np.linalg.det(den)
    t = 1

    if abs(t) < 0.000000000001:
        return -1
    if u >= 0 and v >=0 and u+v <= 1 and t >= 0:
        return 1
    return 0

def add_traingle(A,B,C,traingle_set):
    if((A,B,C) in traingle_set):
        return
    if((A,C,B) in traingle_set):
        return
    if((B,A,C) in traingle_set):
        return
    if((B,C,A) in traingle_set):
        return
    if((C,A,B) in traingle_set):
        return
    if((C,B,A) in traingle_set):
        return
    traingle_set.add((A,B,C))



f=open("traingles.txt","r")
f1=f.readlines()
f.close()

traingle_points = list()

for s in f1:
    s = s[:-1]
    s=s.split()
    s[0] = float(s[0])
    s[1] = float(s[1])
    s[2] = float(s[2])
    traingle_points.append(s)

traingles = list()
total_len = len(traingle_points)
for x in range(0,total_len,3):
    traingles.append([traingle_points[x],traingle_points[x+1],traingle_points[x+2]] )

def cal(data,D):
    O = np.array([data[0],data[1],data[2]])
    count = 0
    
    for traingle in traingles:
        r = Ray()
        r.orig = Vec3(data[0],data[1],data[2])
        r.direction = Vec3(D[0],D[1],D[2]).normalize()
        
        t1 = ray_triangle_intersect(r,
            Vec3(traingle[0][0], traingle[0][1], traingle[0][2]),
            Vec3(traingle[1][0], traingle[1][1], traingle[1][2]),
            Vec3(traingle[2][0], traingle[2][1], traingle[2][2])
            )
        
        t2 = ray_triangle_intersect(r,
            Vec3(traingle[0][0], traingle[0][1], traingle[0][2]),
            Vec3(traingle[2][0], traingle[2][1], traingle[2][2]),
            Vec3(traingle[1][0], traingle[1][1], traingle[1][2])
            )
        
        t = 0
        if(t1 == float('-inf') and t2 == float('-inf')):
            continue

        if(t1 != float('-inf')):
            t = t1
        else:
            t = t2

        if abs(t) < 0.0000001:
            count = -1
            break
        elif t > 0:
            count += 1

    if count >= 0:
        return count%2
    else:
        return 2

print_count = 0
data_set = list()
for traingle in traingles:
    A = np.array([traingle[0][0], traingle[0][1], traingle[0][2]])
    B = np.array([traingle[1][0], traingle[1][1], traingle[1][2]])
    C = np.array([traingle[2][0], traingle[2][1], traingle[2][2]])

    N = np.cross(B-A,C-A)
    N = N / math.sqrt(np.square(N).sum())
    for i in range(0,200):
        x = random.uniform(0, 1)
        y = random.uniform(0, 1)

        h = random.uniform(0,1)

        vx = (C-A) * x + A
        vy = (B-A) * y + A
        vz = h * vx + (1-h) * vy
        data_set.append((vz[0],vz[1],vz[2]))


        new_point = vz + N*0.6
        data_set.append((new_point[0],new_point[1],new_point[2]))
        new_point = vz + N*0.5
        data_set.append((new_point[0],new_point[1],new_point[2]))
        new_point = vz + N*0.25
        data_set.append((new_point[0],new_point[1],new_point[2]))

        new_point = vz - N*0.02
        data_set.append((new_point[0],new_point[1],new_point[2]))
        new_point = vz - N*0.1
        data_set.append((new_point[0],new_point[1],new_point[2]))
        new_point = vz - N*0.125
        data_set.append((new_point[0],new_point[1],new_point[2]))
        new_point = vz - N*0.26
        data_set.append((new_point[0],new_point[1],new_point[2]))

print(len(data_set))
for data in data_set:
    val = [0,0,0]
    cur_val = cal(data,np.array([1,0,0]))
    val[cur_val] = val[cur_val] + 1

    cur_val = cal(data,np.array([0,1,0]))
    val[cur_val] = val[cur_val] + 1

    cur_val = cal(data,np.array([1,0,0]))
    val[cur_val] = val[cur_val] + 1
    
    for ran_d in np.random.rand(5,3):
        cur_val = cal(data,ran_d)
        val[cur_val] = val[cur_val] + 1
        
    if(val[0] >= val[1] and val[0] >= val[2]):
        print("{0}  {1}   {2}    1").format(data[0],data[1],data[2])
    elif(val[2] >= val[1] and val[2] >= val[0]):
        print("{0}  {1}   {2}    0".format(data[0],data[1],data[2]))
    else:
        print("{0}  {1}   {2}   -1").format(data[0],data[1],data[2])
    print_count = print_count + 1

