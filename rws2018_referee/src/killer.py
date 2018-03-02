#!/usr/bin/env python

import rospy
from std_msgs.msg import String
import subprocess

def callback(data):
    subprocess.call(["rosnode", "kill", data.data])
    rospy.loginfo("%s killed", data.data)


if __name__ == '__main__':
    rospy.init_node("hitman")
    rospy.Subscriber("victim", String, callback, queue_size=100)
    rospy.spin()
