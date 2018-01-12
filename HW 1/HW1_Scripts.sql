-- Name: Thomas Noelcke
-- Assignment: Assignment 1 CS 440 winter of 2018.

--Problem 1
--a.

SELECT did, dname 
  FROM dept
  WHERE dept.did
  IN (SELECT did
        FROM works
        WHERE works.pct_time > 99);
 --b.
 
 SELECT ename, age FROM emp where emp.eid IN (SELECT eid FROM
(SELECT eid 
  FROM works as w
  WHERE w.did = (SELECT did FROM dept AS d WHERE dname = 'Software')) AS es
  WHERE eid IN (SELECT eid
  FROM works as w
  WHERE w.did = (SELECT did FROM dept AS d WHERE dname = 'Hardware')));
 
    
  
 
