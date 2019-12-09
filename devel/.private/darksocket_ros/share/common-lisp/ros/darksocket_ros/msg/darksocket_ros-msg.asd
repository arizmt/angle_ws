
(cl:in-package :asdf)

(defsystem "darksocket_ros-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils :darknet_ros_msgs-msg
               :sensor_msgs-msg
)
  :components ((:file "_package")
    (:file "Detections" :depends-on ("_package_Detections"))
    (:file "_package_Detections" :depends-on ("_package"))
  ))