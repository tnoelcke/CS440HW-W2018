DROP TABLE IF EXISTS `employee`;
DROP TABLE IF EXISTS `department`;

CREATE TABLE `department` (
  `did` int(10) NOT NULL,
  `dname` varchar(40) NOT NULL,
  `budget` double NOT NULL,
  `managerid` int(10) NOT NULL,
  PRIMARY KEY (`did`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


--
-- Dumping data for table `dept`
--

INSERT INTO `department` VALUES 
(3,'Human Resources',1439119,110183),
(4,'Software',1174402,110303),
(5,'Hardware',1544653,110511),
(8,'Research',4291505,111692),
(9,'Customer Service',1308515,111692),
(10,'Clerk',2344451,500120),
(12,'Manufacturing',2244123,500160),
(14,'Medicine',3344451,500170),
(20,'Engineering',4124451,500120),
(21,'Finance',1344000,500220),
(22,'Support',1300001,500223),
(24,'Business processing',2312178,500235),
(26,'Customer relationship',1234451,500223),
(28,'Sales',4344407,500235),
(30,'Sales and services',1114451,500240),
(32,'Marketing',2334451,500254),
(33,'Field monitor',2224451,500258),
(35,'Housekeeping',1344098,500260),
(37,'Instruments',1255000,500160),
(40,'Supplies',9876451,500275),
(42,'Electricals',2309851,500300),
(44,'Electronics',2134450,500330),
(46,'Food',2344345,500300),
(48,'Management',8323451,500350),
(50,'Research',7844451,500400),
(52,'Information tech',9344000,500400),
(53,'Labs',1000051,500400),
(55,'Infrastructure',4344451,500350),
(56,'Testing',2300000,500400),
(58,'Packaging',1312551,500170),
(60,'Warehouse',6346751,500400);


--
-- Table structure for table `emp`
--


CREATE TABLE `employee` (
  `eid` int(10) NOT NULL,
  `ename` varchar(40) NOT NULL,
  `age` int(10) NOT NULL,
  `salary` double NOT NULL,
  PRIMARY KEY (`eid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


--
-- Dumping data for table `emp`
--


INSERT INTO `employee` VALUES 
(10042,'Magy Stamatiou',60,81662),
(10059,'Alejandro McAlpine',63,71218),
(10073,'Shir McClurg',62,56473),
(12000,'Alex Dalas',55,54000),
(12010,'Sergio Ravarez',55,64000),
(12020,'Antonio Lavante',34,36000),
(12030,'Tonny Conner',25,44000),
(12100,'Gladys Cooper',32,77000),
(12110,'Rodney Ferreri',37,99000),
(110183,'Shirish Ossenbruggen',63,48291),
(110303,'Krassimir Wegerle',60,1374402),
(110511,'DeForest Hagimont',59,48626),
(111400,'Arie Staelin',57,72446),
(111692,'Tonny Butterworth',62,40000),
(403353,'Shigehito Kropatsch',52,59783),
(499868,'Shaz Bierbaum',60,61802),
(499950,'Weidon Gente',52,54788),
(499998,'Patricia Breugel',60,40000),
(500120,'Leslie Fields',60,400234),
(500140,'Anika Compton',55,140000),
(500150,'Pedro Mcclain',45,543001),
(500155,'Kadin Lane',54,400001),
(500160,'Antonio Wells',55,100111),
(500165,'Jose Underwood',61,40023),
(500168,'Amani Deleon',58,90010),
(500170,'Niko Yates',45,11010),
(500172,'Talon Beasley',56,410001),
(500174,'Judith Whitaker',58,31200),
(500220,'Helena Charles',60,432200),
(500221,'Silas Harding',61,23000),
(500223,'Victor Johnston',62,110090),
(500225,'Viviana Yang',56,23400),
(500230,'Jonah Mendoza',34,40230),
(500232,'Kash Duran',45,45000),
(500235,'Dalia Mata',32,78000),
(500237,'Kody Fowler',46,670000),
(500240,'Aryana Beltran',43,65000),
(500250,'Yamilet Hodges',67,60000),
(500254,'Leonard Oneill',56,40230),
(500258,'Libby Becker',55,88800),
(500260,'Rylee May',57,40450),
(500270,'Ezekiel Stuart',58,43400),
(500275,'Lennon King',59,52900),
(500280,'Byron Rojas',60,400881),
(500300,'Mikaela Benjamin',34,12000),
(500302,'Brylee Sheppard',36,22010),
(500309,'Pierre Robinson',39,98000),
(500330,'Kieran Murillo',40,11100),
(500350,'Drew Morrison',45,90000),
(500400,'Kierra Wallace',60,400100),
(500521,'Kara Oliver',55,30000);