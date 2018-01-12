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