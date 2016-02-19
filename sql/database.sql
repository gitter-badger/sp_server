/*
Navicat MySQL Data Transfer

Source Server         : 127.0.0.1
Source Server Version : 50711
Source Host           : localhost:3306
Source Database       : sp

Target Server Type    : MYSQL
Target Server Version : 50711
File Encoding         : 65001

Date: 2016-02-17 22:39:33
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `cardshop`
-- ----------------------------
DROP TABLE IF EXISTS `cardshop`;
CREATE TABLE `cardshop` (
  `id` char(255) DEFAULT NULL,
  `shop_type` char(255) DEFAULT NULL,
  `rooms` char(255) DEFAULT NULL,
  `name` char(255) DEFAULT NULL,
  `levels` char(255) DEFAULT NULL,
  `gender` char(255) DEFAULT NULL,
  `price` char(255) DEFAULT NULL,
  `card` char(255) DEFAULT NULL,
  `gf` char(255) DEFAULT NULL,
  `level` char(255) DEFAULT NULL,
  `skill` char(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of cardshop
-- ----------------------------

-- ----------------------------
-- Table structure for `chats`
-- ----------------------------
DROP TABLE IF EXISTS `chats`;
CREATE TABLE `chats` (
  `unknown1` int(1) DEFAULT NULL,
  `unknown2` int(1) DEFAULT NULL,
  `sender` char(255) DEFAULT NULL,
  `reciever` char(255) DEFAULT NULL,
  `msg` text
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


-- ----------------------------
-- Table structure for `equipments`
-- ----------------------------
DROP TABLE IF EXISTS `equipments`;
CREATE TABLE `equipments` (
  `usr_id` int(11) NOT NULL AUTO_INCREMENT,
  `eqp_mag` int(11) DEFAULT '-1',
  `eqp_wpn` int(11) DEFAULT '-1',
  `eqp_arm` int(11) DEFAULT '-1',
  `eqp_pet` int(11) DEFAULT '-1',
  `eqp_foot` int(11) DEFAULT '-1',
  `eqp_body` int(11) DEFAULT '-1',
  `eqp_hand1` int(11) DEFAULT '-1',
  `eqp_hand2` int(11) DEFAULT '-1',
  `eqp_face` int(11) DEFAULT '-1',
  `eqp_hair` int(11) DEFAULT '-1',
  `eqp_head` int(11) DEFAULT '-1',
  PRIMARY KEY (`usr_id`)
) ENGINE=InnoDB AUTO_INCREMENT=320 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of equipments
-- ----------------------------

-- ----------------------------
-- Table structure for `items`
-- ----------------------------
DROP TABLE IF EXISTS `items`;
CREATE TABLE `items` (
  `itm_id` int(11) NOT NULL AUTO_INCREMENT,
  `itm_slot` int(11) DEFAULT NULL,
  `itm_usr_id` int(11) DEFAULT NULL,
  `itm_type` int(11) DEFAULT NULL,
  `itm_gf` int(11) DEFAULT NULL,
  `itm_level` int(11) DEFAULT NULL,
  `itm_skill` int(11) DEFAULT NULL,
  PRIMARY KEY (`itm_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of items
-- ----------------------------

-- ----------------------------
-- Table structure for `shop`
-- ----------------------------
DROP TABLE IF EXISTS `shop`;
CREATE TABLE `shop` (
  `itm_gf` int(11) DEFAULT NULL COMMENT 'Currency for Purchases',
  `itm_type` int(11) DEFAULT NULL,
  `itm_quantity` int(11) DEFAULT NULL,
  `itm_code_cost` int(11) DEFAULT NULL,
  `itm_cash_cost` int(11) DEFAULT NULL,
  `itm_coins_cost` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of shop
-- ----------------------------

-- ----------------------------
-- Table structure for `users`
-- ----------------------------
DROP TABLE IF EXISTS `users`;
CREATE TABLE `users` (
  `usr_id` smallint(6) NOT NULL AUTO_INCREMENT,
  `usr_name` char(16) NOT NULL,
  `usr_pw` char(16) NOT NULL,
  `usr_gender` bit(1) DEFAULT b'0',
  `usr_ip` char(16) DEFAULT NULL,
  `usr_char` int(3) DEFAULT '10',
  `usr_points` int(11) DEFAULT '30000',
  `usr_level` int(11) DEFAULT '0',
  `usr_code` int(11) DEFAULT '3000000',
  `usr_coins` int(11) DEFAULT '3000000',
  `usr_cash` int(11) DEFAULT '3000000',
  `usr_water` int(11) DEFAULT '0',
  `usr_fire` int(11) DEFAULT '0',
  `usr_earth` int(11) DEFAULT '0',
  `usr_wind` int(11) DEFAULT '0',
  `usr_nslots` smallint(3) DEFAULT '48' COMMENT 'Maximum slots',
  `usr_wins` int(11) DEFAULT '0',
  `usr_losses` int(11) DEFAULT '0',
  `usr_ko` int(11) DEFAULT '0',
  `usr_down` int(11) DEFAULT '0',
  `usr_scroll1` int(11) DEFAULT '0',
  `usr_scroll2` int(11) DEFAULT '0',
  `usr_scroll3` int(11) DEFAULT '0',
  `usr_mission` smallint(3) DEFAULT '1',
  PRIMARY KEY (`usr_id`),
  UNIQUE KEY `usr_id` (`usr_id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of users
-- ----------------------------

-- ----------------------------
-- Event structure for `Auto Card`
-- ----------------------------
DROP EVENT IF EXISTS `Auto Card`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` EVENT `Auto Card` ON SCHEDULE EVERY 10 SECOND STARTS '2016-02-17 20:11:21' ON COMPLETION NOT PRESERVE ENABLE DO UPDATE users SET usr_wind = 100000
;;
DELIMITER ;
