#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <turtlesim/msg/pose.hpp>
#include <cmath>

class TurtleFollower:public rclcpp::Node
{
public:
    TurtleFollower():Node("turtle_follower")
    {
        // turtle1의 위치를 구독
        turtle1_pose_subscriber_=this->create_subscription<turtlesim::msg::Pose>("/turtle1/pose",10,std::bind(&TurtleFollower::turtle1PoseCallback,this,std::placeholders::_1));

        // turtle2의 위치를 구독
        turtle2_pose_subscriber_=this->create_subscription<turtlesim::msg::Pose>("/turtle2/pose",10,std::bind(&TurtleFollower::turtle2PoseCallback,this, std::placeholders::_1));

        // turtle2의 속도를 퍼블리시
        turtle2_velocity_publisher_=this->create_publisher<geometry_msgs::msg::Twist>("/turtle2/cmd_vel",10);
    }

private:
    void turtle1PoseCallback(const turtlesim::msg::Pose::SharedPtr msg)
    {
        // turtle1의 위치를 업데이트
        turtle1_x_=msg->x;
        turtle1_y_=msg->y;
        turtle1_theta_=msg->theta;

        // turtle2의 위치를 따라가도록 제어 명령을 계산
        followTurtle1();
    }

    void turtle2PoseCallback(const turtlesim::msg::Pose::SharedPtr msg)
    {
        // turtle2의 위치를 업데이트
        turtle2_x_=msg->x;
        turtle2_y_=msg->y;
        turtle2_theta_=msg->theta;
    }

    void followTurtle1()
    {
        // turtle1과 turtle2 간의 거리 계산
        float distance=std::sqrt(std::pow(turtle1_x_-turtle2_x_,2)+std::pow(turtle1_y_-turtle2_y_,2));

        // turtle1과 turtle2가 너무 가까우면 정지 (거리 기준 임계값)
        const float follow_distance=0.5;

        auto twist_msg=geometry_msgs::msg::Twist();

        if (distance > follow_distance){
            // turtle2가 turtle1을 따라가도록 선형 및 각속도 계산
            twist_msg.linear.x=1.5*distance; // 비례 제어
            twist_msg.angular.z=4.0*(std::atan2(turtle1_y_-turtle2_y_,turtle1_x_-turtle2_x_)-turtle2_theta_);
        } else {
            // turtle2가 turtle1에 가까워지면 멈춤
            twist_msg.linear.x=0.0;
            twist_msg.angular.z=0.0;
        }

        // turtle2의 속도 명령 퍼블리시
        turtle2_velocity_publisher_->publish(twist_msg);
    }

    // turtle1의 위치 변수
    float turtle1_x_,turtle1_y_,turtle1_theta_;

    // turtle2의 위치 변수
    float turtle2_x_,turtle2_y_,turtle2_theta_;

    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr turtle1_pose_subscriber_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr turtle2_pose_subscriber_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr turtle2_velocity_publisher_;
};

int main(int argc,char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TurtleFollower>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    
    return 0;
}
