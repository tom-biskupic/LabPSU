
class Linearizer:
    def __init__(self,points):
        self.points = points
        self.num_points = len(points)

    def code_to_value(self,code):
        #
        #   Iterate through the points to find the first
        #   point with a code greater than what we want
        #
        for i in range(0,self.num_points):
            if self.points[i][0] > code:
                if i == 0:
                    return self._interpolate_value(code,(0,0.0),self.points[i])
                else:
                    return self._interpolate_value(code,self.points[i-1],self.points[i])

        #
        #   Otherwise we got to the end of the list so we extrapolate from
        #   The last two points.
        #
        return self._interpolate_value(code,self.points[self.num_points-2],self.points[self.num_points-1])

    def value_to_code(self,value):
        for i in range(0,self.num_points):
            if self.points[i][1] > value:
                if i == 0:
                    return 0
                else:
                    return self._interpolate_code(value,self.points[i-1],self.points[i])

        #
        #   Otherwise we got to the end of the list so we extrapolate from
        #   The last two points.
        #
        return self._interpolate_code(value,self.points[self.num_points-2],self.points[self.num_points-1])

    def _interpolate_value(self,code,point1,point2):
        return (point2[1] - point1[1])/(point2[0] - point1[0]) * (code - point1[0]) + point1[1]

    def _interpolate_code(self,value,point1,point2):
        return round((point2[0] - point1[0])/(point2[1] - point1[1]) * (value - point1[1]) + point1[0])
