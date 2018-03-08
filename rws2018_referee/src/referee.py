#!/usr/bin/env python

import rospy
from visualization_msgs.msg import Marker
from visualization_msgs.msg import MarkerArray
from std_msgs.msg import String
from rws2018_msgs.msg import MakeAPlay
#from rws2018_msgs.srv import GameQuery
import random
import tf
import math
import os
import subprocess
from sensor_msgs.msg import Image, PointCloud2, PointField

import rospkg

score = [0,0,0]
pub_make_a_play = rospy.Publisher('make_a_play', MakeAPlay, queue_size=0)
pub_referee = rospy.Publisher("referee_markers", MarkerArray, queue_size=10)
pub_score = rospy.Publisher("score_markers", MarkerArray, queue_size=10)
pub_rip = rospy.Publisher("kill_markers", MarkerArray, queue_size=10)
pub_killer = rospy.Publisher("victim", String, queue_size=10)

rate = 0
game_duration = rospy.get_param('/game_duration')
positive_score = rospy.get_param('/positive_score')
negative_score = rospy.get_param('/negative_score')
killed = []
teamA = []
teamB = []
teamC = []
player_score_pos = dict()
player_score_neg = dict()
selected_team_count = 0
game_pause = False
over = False
to_print_end = True
def gameQueryCallback(event):
    global teamA, teamB, teamC, selected_team_count, game_pause, score
    game_pause = True 

    rospy.loginfo("gameQueryCallback")
    rospy.loginfo("selected_team_count = " + str(selected_team_count))
    return None

    # percorrer a lista de equipas
    team_list = [teamA, teamB, teamC]
    
    selected_team = team_list[selected_team_count]
    
    #print("team_list is = " + str(team_list)) 
    #print("selected_team is = " + str(selected_team)) 

    # sortear um jogador alive da equipa desta iteracao

    selected_player = random.choice(selected_team)
    #selected_player = "idomingues"
    #print("selected_player is = " + str(selected_player)) 

    # sortear um objeto
    objects = ["banana", "soda_can", "onion", "tomato"]
    selected_object = random.choice(objects)

    rospack = rospkg.RosPack()
    path_pcd = rospack.get_path('rwsua2017_referee') + "/../pcd/"
    file_pcd = path_pcd + selected_object + ".pcd"
    #print("vou ler o " + str(file_pcd))

    # pedir ao pcd2pointcloud para enviar o objeto

    cmd = "rosrun rwsfi2016_referee pcd2pointcloud _input:="+ file_pcd + " _output:=/object_point_cloud /world:=" + selected_player + " _one_shot:=1"
    #print "Executing command: " + cmd
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    for line in p.stdout.readlines():
        print line,
        p.wait()

    # sleep for duration (to make sure people get the point clouds)
    d = rospy.Duration(2, 0)
    rospy.sleep(d)
 
    # chamar o servico game_query
    service_name = "/" + selected_player + "/game_query"
    correct_response = False

    try:
        rospy.wait_for_service(service_name, 1)
    except rospy.ROSException, e:
        print("Perguntei " + selected_object + " ao " + selected_player + " e ele(a) nao deu resposta")
        print("RESPOSTA AUSENTE!  ...")

    try:
        game_query = rospy.ServiceProxy(service_name, GameQuery)
        resp1 = game_query()
        print("Perguntei " + selected_object + " ao " + selected_player + " e ele respondeu " + resp1.resposta)
        # verificar a resposta e afetar a pontuacao
        if selected_object == resp1.resposta:
            print("RESPOSTA CERTA! FANTASTICO")
            correct_response = True
        else:
            print("RESPOSTA ERRADA! NAO PERCEBES NADA DISTO ...")
    except rospy.ServiceException, e:
        print "Service call failed: %s"%e


    print("score before:" + str(score))
    if correct_response == True:
        score[selected_team_count] = score[selected_team_count] + 5 
    else:
        score[selected_team_count] = score[selected_team_count] - 5 



    print("score after:" + str(score))

    if selected_team_count == 2:
        selected_team_count = 0
    else:
        selected_team_count = selected_team_count + 1

    # sleep for duration (to make sure people get the point clouds)
    rospy.sleep(d)
    game_pause = False 

def timerCallback(event):
    global game_pause
    global over
    if game_pause == True or over:
        if over:
            printScores()
        return 

    a = MakeAPlay()

    global teamA, teamB, teamC
    global killed
    print("killed: " + str(killed))

    if len(killed)==0:
        players_killed = []
    else:
        players_killed = [i[0] for i in killed]

    for player in teamA:
        if player in players_killed:
            a.red_dead.append(player)
        else:
            a.red_alive.append(player)

    for player in teamB:
        if player in players_killed:
            a.green_dead.append(player)
        else:
            a.green_alive.append(player)

    for player in teamC:
        if player in players_killed:
            a.blue_dead.append(player)
        else:
            a.blue_alive.append(player)

    #cheetah
    md = 0.1
    a.cheetah = random.random()/10
    a.dog = random.random()/10
    a.cat = random.random()/10
    a.turtle = random.random()/10
    if not rospy.is_shutdown():
        global pub_make_a_play
        pub_make_a_play.publish(a)

def printScores():
    global to_print_end
    if to_print_end:
        for player,score in player_score_neg.iteritems():
            rospy.loginfo(player + " died: " + str(score))
        for player,score in player_score_pos.iteritems():
            rospy.loginfo(player + " killed: " +  str(score))
        to_print_end = False


def gameEndCallback(event):

    rospy.loginfo("Game finished")
    global pub_score
    global over
    over = True
    ma = MarkerArray()
    

    m1 = Marker()
    m1.header.frame_id = "/world"
    m1.type = m1.TEXT_VIEW_FACING
    m1.action = m1.ADD
    m1.id = 777;
    m1.scale.x = 0.2
    m1.scale.y = 0.2
    m1.scale.z = 0.9
    m1.color.a = 1.0
    m1.color.r = 0.1
    m1.color.g = 0.1
    m1.color.b = 0.1
    m1.pose.position.x = .5
    m1.pose.position.y = 4.5
    m1.pose.position.z = 0 

    if score[0] > score[1] and score[0] > score[2]:
        m1.text = "Team R wins the game"
    elif score[1] > score[0] and score[1] > score[2]:
        m1.text = "Team G wins the game"
    elif score[2] > score[0] and score[2] > score[1]:
        m1.text = "Team B wins the game"
    else:
        m1.text = "WHAT HAPPENNED?"

    
    ma.markers.append(m1)
    pub_score.publish(ma)
    rate.sleep()


    rospy.spin()

    rospy.signal_shutdown("Game finished")

def talker():
    rospy.init_node('referee', anonymous=True)
    listener = tf.TransformListener()
    broadcaster = tf.TransformBroadcaster()
    global rate 
    rate = rospy.Rate(2) # 10hz
    rate.sleep()

    hunting_distance = rospy.get_param('/hunting_distance')
    global teamA
    teamA = rospy.get_param('/team_red')
    global teamB
    teamB = rospy.get_param('/team_green')
    global teamC
    teamC = rospy.get_param('/team_blue')
    for player in teamA:
        player_score_neg[player] = 0
        player_score_pos[player] = 0
    for player in teamB:
        player_score_neg[player] = 0
        player_score_pos[player] = 0
    for player in teamC:
        player_score_neg[player] = 0
        player_score_pos[player] = 0
    
    rospy.Timer(rospy.Duration(0.1), timerCallback, oneshot=False)
    rospy.Timer(rospy.Duration(game_duration), gameEndCallback, oneshot=True)

    #rospy.Timer(rospy.Duration(5), gameQueryCallback, oneshot=False)

    game_start = rospy.get_time()

    while not rospy.is_shutdown():
        if over:
            continue

        #print killed
        tic = rospy.Time.now()
        for i in killed:
            d = tic-i[1]
            if d.to_sec() > 10:
                rospy.logwarn("Ressuscitating %s", i[0])
                killed.remove(i)
                broadcaster.sendTransform((random.random()*10 -5, random.random()*10 -5, 0), tf.transformations.quaternion_from_euler(0, 0, 0), tic, i[0], "/world")


        #print killed
        #rospy.loginfo("Checking ...")

        to_be_killed_A = [];
        to_be_killed_B = [];
        to_be_killed_C = [];
        out_of_arena_A = [];
        out_of_arena_B = [];
        out_of_arena_C = [];
        max_distance_from_center_of_arena = 8

        #Checking if players don't stray from the arena
        for player in teamA:
            #check if hunter is killed
            result = [item for item in killed if item[0] == player]
            if not result:
                try:
                    (trans,rot) = listener.lookupTransform("/world", player, rospy.Time(0))
                    distance = math.sqrt(trans[0]*trans[0] + trans[1]*trans[1])
                    #rospy.loginfo("D from %s to %s is %f", hunter, prey, distance)
                    if distance > max_distance_from_center_of_arena:
                        out_of_arena_A.append(player);
                        player_score_neg[player] += 1
                        
                except (tf.LookupException, tf.ConnectivityException, tf.ExtrapolationException):
                    t=1
                    #rospy.loginfo("Referee: tf error")

        for player in teamB:
            #check if hunter is killed
            result = [item for item in killed if item[0] == player]
            if not result:
                try:
                    (trans,rot) = listener.lookupTransform("/world", player, rospy.Time(0))
                    distance = math.sqrt(trans[0]*trans[0] + trans[1]*trans[1])
                    #rospy.loginfo("D from %s to %s is %f", hunter, prey, distance)
                    if distance > max_distance_from_center_of_arena:
                        out_of_arena_B.append(player);
                        player_score_neg[player] += 1
                except (tf.LookupException, tf.ConnectivityException, tf.ExtrapolationException):
                    t=1
                    #rospy.loginfo("Referee: tf error")

        for player in teamC:
            #check if hunter is killed
            result = [item for item in killed if item[0] == player]
            if not result:
                try:
                    (trans,rot) = listener.lookupTransform("/world", player, rospy.Time(0))
                    distance = math.sqrt(trans[0]*trans[0] + trans[1]*trans[1])
                    #rospy.loginfo("D from %s to %s is %f", hunter, prey, distance)
                    if distance > max_distance_from_center_of_arena:
                        out_of_arena_C.append(player);
                        player_score_neg[player] += 1
                except (tf.LookupException, tf.ConnectivityException, tf.ExtrapolationException):
                    t=1
                    #rospy.loginfo("Referee: tf error")

        rospy.loginfo("to_be_killed is %s", str(to_be_killed_B))

        #Check if anyone is hunted

        #Team A hunting team B
        for hunter in teamA:
            
            #check if hunter is killed
            result = [item for item in killed if item[0] == hunter]
            if not result:
                for prey in teamB:
                    try:
                        (trans,rot) = listener.lookupTransform(hunter, prey, rospy.Time(0))
                        distance = math.sqrt(trans[0]*trans[0] + trans[1]*trans[1])
                        #rospy.loginfo("D from %s to %s is %f", hunter, prey, distance)
                        if distance < hunting_distance:
                            to_be_killed_A.append(prey);
                            player_score_neg[prey] += 1
                            player_score_pos[hunter] += 1
                    except (tf.LookupException, tf.ConnectivityException, tf.ExtrapolationException):
                        t=1
                        #rospy.loginfo("Referee: tf error")

        #Team A hunting team B
        for hunter in teamB:
            #check if hunter is killed
            result = [item for item in killed if item[0] == hunter]
            if not result:
                for prey in teamC:
                    try:
                        (trans,rot) = listener.lookupTransform(hunter, prey, rospy.Time(0))
                        distance = math.sqrt(trans[0]*trans[0] + trans[1]*trans[1])
                        #rospy.loginfo("D from %s to %s is %f", hunter, prey, distance)
                        if distance < hunting_distance:
                            to_be_killed_B.append(prey);
                            player_score_neg[prey] += 1
                            player_score_pos[hunter] += 1
                    except (tf.LookupException, tf.ConnectivityException, tf.ExtrapolationException):
                        t=1
                        #rospy.loginfo("Referee: tf error")

        #Team C hunting team A
        for hunter in teamC:
            #check if hunter is killed
            result = [item for item in killed if item[0] == hunter]
            if not result:
                for prey in teamA:
                    try:
                        (trans,rot) = listener.lookupTransform(hunter, prey, rospy.Time(0))
                        distance = math.sqrt(trans[0]*trans[0] + trans[1]*trans[1])
                        #rospy.loginfo("D from %s to %s is %f", hunter, prey, distance)
                        if distance < hunting_distance:
                            to_be_killed_C.append(prey);
                            player_score_neg[prey] += 1
                            player_score_pos[hunter] += 1
                    except (tf.LookupException, tf.ConnectivityException, tf.ExtrapolationException):
                        t=1
                        #rospy.loginfo("Referee: tf error")


        ma_killed = MarkerArray()

        kill_time = rospy.Time.now()
        for tbk in to_be_killed_A:
            #rospy.logwarn("%s is to be killed by Team A", tbk)

            #Check if tbk is in the list of killed players
            found = False
            for item in killed:
                if item[0] == tbk:
                    found = True
            
            if found == False:
                killed.append((tbk,kill_time))
                rospy.logwarn("Red hunted %s", tbk)
                score[0] = score[0] + positive_score
                score[1] = score[1] + negative_score
                s = String()
                s.data = tbk
                pub_killer.publish(s)
                mk1 = Marker()
                mk1.header.frame_id = "/world"
                (trans,rot) = listener.lookupTransform("/world", tbk, rospy.Time(0))
                mk1.pose.position.x = trans[0]
                mk1.pose.position.y = trans[1]
                mk1.type = mk1.TEXT_VIEW_FACING
                mk1.action = mk1.ADD
                mk1.id = 0;
                mk1.ns = tbk
                mk1.scale.z = 0.4
                mk1.color.a = 1.0
                mk1.text = tbk
                mk1.lifetime = rospy.Duration.from_sec(5)
                mk1.frame_locked = 0
                ma_killed.markers.append(mk1)

                broadcaster.sendTransform((-100, -100, 0), tf.transformations.quaternion_from_euler(0, 0, 0), kill_time, tbk, "/world")


            else:
                t=1
                #rospy.logwarn("%s was already hunted", tbk)


        for tbk in to_be_killed_B:
            #rospy.logwarn("%s is to be killed by Team B", tbk)

            #Check if tbk is in the list of killed players
            found = False
            for item in killed:
                if item[0] == tbk:
                    found = True
            
            if found == False:
                killed.append((tbk,kill_time))
                rospy.logwarn("Green hunted %s", tbk)
                score[1] = score[1] + positive_score
                score[2] = score[2] + negative_score
                s = String()
                s.data = tbk
                pub_killer.publish(s)

                mk1 = Marker()
                mk1.header.frame_id = "/world"
                (trans,rot) = listener.lookupTransform("/world", tbk, rospy.Time(0))
                mk1.pose.position.x = trans[0]
                mk1.pose.position.y = trans[1]
                mk1.type = mk1.TEXT_VIEW_FACING
                mk1.action = mk1.ADD
                mk1.id = 0;
                mk1.ns = tbk
                mk1.scale.z = 0.4
                mk1.color.a = 1.0
                mk1.text = tbk
                mk1.lifetime = rospy.Duration.from_sec(5)
                mk1.frame_locked = 0
                ma_killed.markers.append(mk1)
                broadcaster.sendTransform((100, -100, 0), tf.transformations.quaternion_from_euler(0, 0, 0), kill_time, tbk, "/world")

            else:
                t=1
                #rospy.logwarn("%s was already hunted", tbk)

            
        for tbk in to_be_killed_C:
            #rospy.logwarn("%s is to be killed by Team C", tbk)

            #Check if tbk is in the list of killed players
            found = False
            for item in killed:
                if item[0] == tbk:
                    found = True
            
            if found == False:
                killed.append((tbk,kill_time))
                rospy.logwarn("Blue hunted %s", tbk)
                score[2] = score[2] + positive_score
                score[0] = score[0] + negative_score
                s = String()
                s.data = tbk
                pub_killer.publish(s)


                mk1 = Marker()
                mk1.header.frame_id = "/world"
                (trans,rot) = listener.lookupTransform("/world", tbk, rospy.Time(0))
                mk1.pose.position.x = trans[0]
                mk1.pose.position.y = trans[1]
                mk1.type = mk1.TEXT_VIEW_FACING
                mk1.action = mk1.ADD
                mk1.id = 0;
                mk1.ns = tbk
                mk1.scale.z = 0.4
                mk1.color.a = 1.0
                mk1.text = tbk
                mk1.lifetime = rospy.Duration.from_sec(5)
                mk1.frame_locked = 0
                ma_killed.markers.append(mk1)
                broadcaster.sendTransform((100, 100, 0), tf.transformations.quaternion_from_euler(0, 0, 0), kill_time, tbk, "/world")

            else:
                t=1
                #rospy.logwarn("%s was already hunted", tbk)
        
        #--------------------------------
        #Killing because of stray from arena
        #--------------------------------
        for tbk in out_of_arena_A:
            #rospy.logwarn("%s is to be killed by Team A", tbk)

            #Check if tbk is in the list of killed players
            found = False
            for item in killed:
                if item[0] == tbk:
                    found = True
            
            if found == False:
                killed.append((tbk,kill_time))
                rospy.logwarn("Red hunted %s", tbk)
                score[0] = score[0] + negative_score
                s = String()
                s.data = tbk
                pub_killer.publish(s)
                mk1 = Marker()
                mk1.header.frame_id = "/world"
                (trans,rot) = listener.lookupTransform("/world", tbk, rospy.Time(0))
                mk1.pose.position.x = trans[0]
                mk1.pose.position.y = trans[1]
                mk1.type = mk1.TEXT_VIEW_FACING
                mk1.action = mk1.ADD
                mk1.id = 0;
                mk1.ns = tbk
                mk1.scale.z = 0.4
                mk1.color.a = 1.0
                mk1.text = "Queres fugir " + tbk + "?"
                mk1.lifetime = rospy.Duration.from_sec(5)
                mk1.frame_locked = 0
                ma_killed.markers.append(mk1)

                broadcaster.sendTransform((-100, -100, 0), tf.transformations.quaternion_from_euler(0, 0, 0), kill_time, tbk, "/world")


            else:
                t=1
                #rospy.logwarn("%s was already hunted", tbk)


        for tbk in out_of_arena_B:
            #rospy.logwarn("%s is to be killed by Team B", tbk)

            #Check if tbk is in the list of killed players
            found = False
            for item in killed:
                if item[0] == tbk:
                    found = True
            
            if found == False:
                killed.append((tbk,kill_time))
                rospy.logwarn("Green hunted %s", tbk)
                score[1] = score[1] + negative_score
                s = String()
                s.data = tbk
                pub_killer.publish(s)

                mk1 = Marker()
                mk1.header.frame_id = "/world"
                (trans,rot) = listener.lookupTransform("/world", tbk, rospy.Time(0))
                mk1.pose.position.x = trans[0]
                mk1.pose.position.y = trans[1]
                mk1.type = mk1.TEXT_VIEW_FACING
                mk1.action = mk1.ADD
                mk1.id = 0;
                mk1.ns = tbk
                mk1.scale.z = 0.4
                mk1.color.a = 1.0
                mk1.text = "Queres fugir " + tbk + "?"
                mk1.lifetime = rospy.Duration.from_sec(5)
                mk1.frame_locked = 0
                ma_killed.markers.append(mk1)
                broadcaster.sendTransform((100, -100, 0), tf.transformations.quaternion_from_euler(0, 0, 0), kill_time, tbk, "/world")

            else:
                t=1
                #rospy.logwarn("%s was already hunted", tbk)

            
        for tbk in out_of_arena_C:
            #rospy.logwarn("%s is to be killed by Team C", tbk)

            #Check if tbk is in the list of killed players
            found = False
            for item in killed:
                if item[0] == tbk:
                    found = True
            
            if found == False:
                killed.append((tbk,kill_time))
                rospy.logwarn("Blue hunted %s", tbk)
                score[2] = score[2] + negative_score
                s = String()
                s.data = tbk
                pub_killer.publish(s)

                mk1 = Marker()
                mk1.header.frame_id = "/world"
                (trans,rot) = listener.lookupTransform("/world", tbk, rospy.Time(0))
                mk1.pose.position.x = trans[0]
                mk1.pose.position.y = trans[1]
                mk1.type = mk1.TEXT_VIEW_FACING
                mk1.action = mk1.ADD
                mk1.id = 0;
                mk1.ns = tbk
                mk1.scale.z = 0.4
                mk1.color.a = 1.0
                mk1.text = "Queres fugir " + tbk + "?"
                mk1.lifetime = rospy.Duration.from_sec(5)
                mk1.frame_locked = 0
                ma_killed.markers.append(mk1)
                broadcaster.sendTransform((100, 100, 0), tf.transformations.quaternion_from_euler(0, 0, 0), kill_time, tbk, "/world")

            else:
                t=1
                #rospy.logwarn("%s was already hunted", tbk)




        if ma_killed.markers:
            pub_rip.publish(ma_killed)
        #print killed
                        

        #rospy.logwarn("Team A: %s hunted %s", hunter, prey)
        ##os.system('rosnode kill ' + prey)
        #score[0] = score[0] + positive_score
        #score[1] = score[1] + negative_score
        #s = String()
        #s.data = prey
        #pub_killer.publish(s)


        ma = MarkerArray()

        m1 = Marker()
        m1.header.frame_id = "/world"
        m1.type = m1.TEXT_VIEW_FACING
        m1.action = m1.ADD
        m1.id = 0;
        m1.scale.x = 0.2
        m1.scale.y = 0.2
        m1.scale.z = 0.6
        m1.color.a = 1.0
        m1.color.r = 1.0
        m1.color.g = 0.0
        m1.color.b = 0.0
        m1.text = "R=" + str(score[0])
        m1.pose.position.x = -5.0
        m1.pose.position.y = 5.2
        m1.pose.position.z = 0 
        ma.markers.append(m1)

        m2 = Marker()
        m2.header.frame_id = "/world"
        m2.type = m2.TEXT_VIEW_FACING
        m2.action = m2.ADD
        m2.id = 1;
        m2.scale.x = 0.2
        m2.scale.y = 0.2
        m2.scale.z = 0.6
        m2.color.a = 1.0
        m2.color.r = 0.0
        m2.color.g = 1.0
        m2.color.b = 0.0
        m2.text = "G=" + str(score[1])
        m2.pose.position.x = -3.0
        m2.pose.position.y = 5.2
        m2.pose.position.z = 0 
        ma.markers.append(m2)

        m3 = Marker()
        m3.header.frame_id = "/world"
        m3.type = m3.TEXT_VIEW_FACING
        m3.action = m3.ADD
        m3.id = 2;
        m3.scale.x = 0.2
        m3.scale.y = 0.2
        m3.scale.z = 0.6
        m3.color.a = 1.0
        m3.color.r = 0.0
        m3.color.g = 0.0
        m3.color.b = 1.0
        m3.text = "B=" + str(score[2])
        m3.pose.position.x = -1.0
        m3.pose.position.y = 5.2
        m3.pose.position.z = 0 
        ma.markers.append(m3)

        m4 = Marker()
        m4.header.frame_id = "/world"
        m4.type = m4.TEXT_VIEW_FACING
        m4.action = m4.ADD
        m4.id = 3;
        m4.scale.x = 0.2
        m4.scale.y = 0.2
        m4.scale.z = 0.6
        m4.color.a = 1.0
        m4.color.r = 0.0
        m4.color.g = 0.0
        m4.color.b = 1.0
        #,c rospy.Time(

        time_now = rospy.get_time()

        m4.text = "Time " + str(format(time_now-game_start, '.2f')) + " of " + str(game_duration)
        m4.pose.position.x = 3.5
        m4.pose.position.y = 5.2
        m4.pose.position.z = 0 
        ma.markers.append(m4)

        pub_score.publish(ma)
        rate.sleep()

if __name__ == '__main__':
    try:
        talker()
    except rospy.ROSInterruptException:
        pass
