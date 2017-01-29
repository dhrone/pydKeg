-- MySQL dump 10.13  Distrib 5.7.12, for osx10.9 (x86_64)
--
-- Host: pydKeg-master.local    Database: pydKeg
-- ------------------------------------------------------
-- Server version	5.5.54-0+deb7u1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `Beer`
--
CREATE SCHEMA IF NOT EXISTS `pydKeg`;
USE `pydKeg`;

DROP TABLE IF EXISTS `Beer`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Beer` (
  `idBeer` int(11) NOT NULL AUTO_INCREMENT,
  `Name` tinytext NOT NULL,
  `ABV` float DEFAULT NULL,
  `IBU` float DEFAULT NULL,
  `Color` float DEFAULT NULL,
  `OriginalGravity` float DEFAULT NULL,
  `FinalGravity` float DEFAULT NULL,
  `Description` text,
  `Brewed` datetime DEFAULT NULL,
  `Kegged` datetime DEFAULT NULL,
  `Tapped` datetime DEFAULT NULL,
  `Notes` text,
  PRIMARY KEY (`idBeer`),
  UNIQUE KEY `idBeer_UNIQUE` (`idBeer`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Beer`
--

LOCK TABLES `Beer` WRITE;
/*!40000 ALTER TABLE `Beer` DISABLE KEYS */;
INSERT INTO `Beer` VALUES (1,'Porter',8.1,24,30,1.072,1.01,'Low Alpha acid German and Slovenian hobs make this an easy-drinking beer.  Slightly dry, this beer hosts several specialty grains that provide you with different flavors while drinking','2016-10-22 00:00:00','2016-12-11 00:00:00','2016-12-28 00:00:00','This beer was specially produced to honor my good friend Sharon. It has become a great tradition to ensure I dedicate one brew per year to her simply because she’s great to drink with, and…we all love her!'),(2,'Apollo/Citra IPA',7.2,73,9.8,1.061,1.006,'High Alpha acid Apollo hops gives this beer a strong bitterness, but it is compensated by the great flavors from the late-addition Citra hops.','2016-10-17 00:00:00','2016-12-11 00:00:00','2016-12-31 00:00:00',NULL),(3,'Jalapeno Ale',6,32,7.7,1.057,1.011,'Brewed with hot Jalapeno peppers from Tagg’s garden. Tagg roasted, toasted, then froze the peppers to ensure all of the capsaicin and flavors were released. Their addition to the secondary fermentation gives this beer a nice hint of Jalapeno flavor and a slight spiciness that accumulates the more you drink.','2016-09-27 00:00:00','2016-11-16 00:00:00','2016-12-05 00:00:00','Made because Tagg didn’t know what to do with the over-abundance of peppers his garden produced this year.'),(4,'IPA',6.9,49,7.4,1.064,1.011,'A great run-of-the-mill IPA with just the right amount of dryness to make it easy to drink.','2016-05-10 00:00:00','2016-07-07 00:00:00','2016-12-24 00:00:00',NULL),(5,'Rye Pale Ale',6.4,29,4.2,1.058,1.009,'Though the cherry didn’t “pop” with this beer, the breadiness of the Rye definitely shines.  A well-balanced beer.','2016-07-12 00:00:00','2016-09-14 00:00:00','2016-11-14 00:00:00','If you like Rye or Pumpernickel bread, you’ll love this beer!');
/*!40000 ALTER TABLE `Beer` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Consumption`
--

DROP TABLE IF EXISTS `Consumption`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Consumption` (
  `idConsumption` int(11) NOT NULL,
  `timestamp` datetime NOT NULL,
  `Beer` int(11) NOT NULL,
  `Remaining` float NOT NULL,
  PRIMARY KEY (`idConsumption`),
  KEY `FK_BeerRemaining_idx` (`Beer`),
  CONSTRAINT `FK_BeerRemaining` FOREIGN KEY (`Beer`) REFERENCES `Beer` (`idBeer`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Consumption`
--

LOCK TABLES `Consumption` WRITE;
/*!40000 ALTER TABLE `Consumption` DISABLE KEYS */;
/*!40000 ALTER TABLE `Consumption` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Tap`
--

DROP TABLE IF EXISTS `Tap`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Tap` (
  `Tap` int(11) NOT NULL,
  `Beer` int(11) NOT NULL,
  PRIMARY KEY (`Tap`),
  UNIQUE KEY `Tap_UNIQUE` (`Tap`),
  KEY `FK_Despensing_idx` (`Beer`),
  CONSTRAINT `FK_Despensing` FOREIGN KEY (`Beer`) REFERENCES `Beer` (`idBeer`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Tap`
--

LOCK TABLES `Tap` WRITE;
/*!40000 ALTER TABLE `Tap` DISABLE KEYS */;
/*!40000 ALTER TABLE `Tap` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2017-01-28 17:09:52
