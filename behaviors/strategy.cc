#include "naobehavior.h"
#include "../rvdraw/rvdraw.h"

extern int agentBodyType;


SkillType NaoBehavior::selectSkill() {
    return kickingAndStand();
}

SkillType NaoBehavior::kickingAndStand() {
    const double MAX_DIST = 10000.0;
    // Find closest player to ball
    int playerClosestToBall = -1;
    double closestDistanceToBall = MAX_DIST;
    for(int i = WO_TEAMMATE1; i < WO_TEAMMATE1+NUM_AGENTS; ++i) {
        VecPosition temp;
        int playerNum = i - WO_TEAMMATE1 + 1;
        if (worldModel->getUNum() == playerNum) {
            // This is us
            temp = worldModel->getMyPosition();
        } else {
            WorldObject* teammate = worldModel->getWorldObject( i );
            // if (worldModel->getFallenTeammate(playerNum))
            //     cout << worldModel->getUNum() << ": Player " << playerNum << " is fallen" << endl;
            if (teammate->validPosition && worldModel->getFallenTeammate(playerNum) != true) {
                temp = teammate->pos;
            } else {
                continue;
            }
        }
        temp.setZ(0);
        double distanceToBall = temp.getDistanceTo(ball);

        if (distanceToBall < closestDistanceToBall) {
            if (worldModel->getUNum() == 1){
                if ( ball.getY() > -1.5 && ball.getY() < 1.5 && ball.getX() < -HALF_FIELD_X + 2){
                    playerClosestToBall = playerNum;
                    closestDistanceToBall = distanceToBall;
                }
            } else {
                playerClosestToBall = playerNum;
                closestDistanceToBall = distanceToBall;
            }
        }
    }

    if (playerClosestToBall == worldModel->getUNum()) {
        // Have closest player kick the ball toward the center
        VecPosition mypos = worldModel->getMyPosition();
        double close_dist = 0xff;
        for(int i = WO_OPPONENT1; i < WO_OPPONENT1+NUM_AGENTS; ++i) {
            WorldObject* opponent = worldModel->getWorldObject( i );
            double tmp;
            if (opponent->validPosition) {
                tmp = mypos.getDistanceTo(opponent->pos);
            } else {
                tmp = 0xff;
            }
            if (close_dist > tmp ){
                close_dist = tmp;
            }
        }
        if (close_dist < 1){
            return kickBall(KICK_IK, VecPosition(HALF_FIELD_X+1, 0, 0));
        } else {
            return kickBall(KICK_FORWARD, VecPosition(HALF_FIELD_X+1, 0, 0));
        }
    } else {

        VecPosition target = this->defaultPosition();
        // Adjust target to not be too close to teammates or the ball
        target = collisionAvoidance(true /*teammate*/, false/*opponent*/, true/*ball*/, 1/*proximity thresh*/, .5/*collision thresh*/, target, true/*keepDistance*/);

        if (me.getDistanceTo(target) < .25) {
            // Close enough to desired position and orientation so just stand
            return SKILL_STAND;
        } else if (me.getDistanceTo(target) < .5) {
            // Close to desired position so start turning to face center
            return goToTargetRelative(worldModel->g2l(target), 0);
        } else {
            // Move toward target location
            return goToTarget(target);
        }
    }
}
