import time

from .. import NogginConstants as Constants
from ..util.MyMath import (dist, getRelativeBearing,
                          getRelativeVelocityX,
                          getRelativeVelocityY,
                          getRelativeX,
                          getRelativeY)


class Ball:
    """
    Class for holding all current Ball information, contains:

    -centerX,centerY -- center (x,y) coordinates of ball on image screen
    -angleX,angleY -- angles of center (x,y) to focal point in degrees
    -dist -- distance from center of the body to ball in cms
    -bearing -- angle in x-axis from center of body to ball
    -elevation -- angle in y-axis from center of body to ball
    -framesOn -- # of consecutive frames the ball has been recognized in vision
    -framesOff -- # of consecutive frames the ball has been not recognized
    NOTE: if framesOn > 0, framesOff == 0, and vice versa
    -x,y -- (x,y) coordinate of ball in cms of dog on field via ekf
    -uncertX,uncertY -- uncertainty in x,y axis from ekf
    -velX,velY -- velocity in x,y axis of ball from ekf
    -locDist -- euclidian distance between (x,y) of self to (x,y) of ball (ekf)
    -locBearing -- relative bearing between self (x,y) and ball (x,y) via ekf
    -on -- simple bool if ball is on vision frame or not
    """
    def __init__(self, visionBall):
        (self.centerX,
         self.centerY,
         self.width,
         self.height,
         self.angleX,
         self.angleY,
         self.focDist,
         self.confidence,
         self.dist,
         self.bearing,
         self.elevation,
         self.framesOn,
         self.framesOff,
         self.prevFramesOn,
         self.prevFramesOff,
         self.x,
         self.y,
         self.uncertX,
         self.uncertY,
         self.sd,
         self.velX,
         self.velY,
         self.uncertVelX,
         self.uncertVelY,
         self.locDist,
         self.locBearing,
         self.locRelX,
         self.locRelY, # loc based
         self.relX, # vision based
         self.relY,
         self.relVelX,
         self.relVelY,
         self.lastVisionDist,
         self.lastVisionBearing,
         self.lastVisionCenterX,
         self.lastVisionCenterY,
         self.lastVisionAngleX,
         self.lastVisionAngleY,
         self.lastTimeSeen,
         self.lastSeenDist,
         self.lastSeenBearing,
         self.on) = [0]*Constants.NUM_TOTAL_BALL_VALUES

        self.updateVision(visionBall)

    def updateVision(self,visionBall):
        """update method gets list of vision updated information"""
        # Hold our history
        self.lastVisionDist = self.dist
        self.lastVisionBearing = self.bearing
        self.lastVisionCenterX = self.centerX
        self.lastVisionCenterY = self.centerY
        self.lastVisionAngleX = self.angleX
        self.lastVisionAngleY = self.angleY

        if self.dist > 0:
            self.lastSeenBearing = self.bearing
            self.lastSeenDist = self.dist

        # Now update to the new stuff
        self.centerX = visionBall.centerX
        self.centerY = visionBall.centerY
        self.width = visionBall.width
        self.height = visionBall.height
        self.focDist = visionBall.focDist
        self.dist = visionBall.dist
        self.bearing = visionBall.bearing
        self.elevation = visionBall.elevation
        self.confidence = visionBall.confidence

        # set angleX, angleY so that we don't create c->python object overhead
        if self.dist > 0:
            self.angleX = (((Constants.IMAGE_WIDTH/2.-1) - self.centerX)/
                           Constants.IMAGE_ANGLE_X)
            self.angleY = (((Constants.IMAGE_HEIGHT/2.-1) - self.centerY)/
                           Constants.IMAGE_ANGLE_Y)

            self.reportBallSeen()
            if not self.on:
                self.prevFramesOff = self.framesOff
            self.on = True
            self.framesOn += 1
            self.framesOff = 0
            self.relX = getRelativeX(self.dist, self.bearing)
            self.relY = getRelativeY(self.dist, self.bearing)
        else:
            self.angleX = 0
            self.angleY = 0
            if self.on:
                self.prevFramesOn = self.framesOn
            self.on = False
            self.framesOff += 1
            self.framesOn = 0
            self.relX = 0.0
            self.relY = 0.0

    def reportBallSeen(self):
        """
        Reset the time since seen.  Happens when we see a ball or when
        a teammate tells us he did.
        """
        self.lastTimeSeen = time.time()

    def timeSinceSeen(self):
        """
        Update the time since we last saw a ball
        """
        return (time.time() - self.lastTimeSeen)

    def updateLoc(self, loc, my):
        """
        Update all of our inforamtion pased on the newest localization info
        """
        # Get latest estimates
        if my.teamColor == Constants.TEAM_BLUE:
            self.x = loc.ballX
            self.y = loc.ballY
            self.velX = loc.ballVelX
            self.velY = loc.ballVelY
        else:
            self.x = Constants.FIELD_GREEN_WIDTH - loc.ballX
            self.y = Constants.FIELD_GREEN_HEIGHT - loc.ballY
            self.velX = -loc.ballVelX
            self.velY = -loc.ballVelY

        self.uncertX = loc.ballXUncert
        self.uncertY = loc.ballYUncert
        self.uncertVelX = loc.ballVelXUncert
        self.uncertVelY = loc.ballVelYUncert
        self.sd = self.uncertX * self.uncertY

        # Determine other values
        self.locDist = dist(my.x, my.y, self.x, self.y)
        self.locBearing = getRelativeBearing(my.x, my.y, my.h,
                                             self.x, self.y)
        self.locRelX = getRelativeX(self.locDist, self.locBearing)
        self.locRelY = getRelativeY(self.locDist, self.locBearing)
        self.relVelX = getRelativeVelocityX(my.h, self.velX, self.velY)
        self.relVelY = getRelativeVelocityY(my.h, self.velX, self.velY)

    def __str__(self):
        """returns string with all class variables"""
        return ("vision dist: %g bearing: %g elevation: %g center: (%d,%d) aX/aY: %g/%g, framesOn: %d framesOff: %d on: %s\n loc: (%g,%g) uncert: (%g,%g) sd: %g vel (%g,%g) dist: %g bearing: %g" %
                (self.dist, self.bearing, self.elevation,
                 self.centerX, self.centerY, self.angleX, self.angleY,
                 self.framesOn, self.framesOff, self.on,
                 self.x,self.y,self.uncertX,self.uncertY,self.sd,
                 self.velX,self.velY, self.locDist, self.locBearing))
