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
 -- Question b.
 
 SELECT ename, age FROM emp WHERE emp.eid IN 
	(SELECT eid FROM
		(SELECT eid 
			FROM works as w
				WHERE w.did = 
					(SELECT did 
					 FROM dept AS d 
					 WHERE dname = 'Software')) AS es
				WHERE eid IN 
					(SELECT eid
					 FROM works as w
					 WHERE w.did = 
					 (SELECT did 
					 FROM dept AS d 
					 WHERE dname = 'Hardware')));
 
    
  
 
--Question C
SELECT emp.ename 
  FROM emp
  WHERE emp.eid NOT IN
  (SELECT es.eid 
    FROM (SELECT emp.eid, emp.salary, works.did
    FROM emp, works
    WHERE emp.eid = works.eid) AS es
    WHERE es.salary > (SELECT budget from dept WHERE dept.did = es.did));
  


  
 -- Question D
SELECT managerid FROM dept
	GROUP BY dept.managerid
	HAVING sum(budget) > 5000000 

	
-- Question E
SELECT did, COUNT(eid)
  FROM WORKS
  GROUP BY did
  HAVING SUM(pct_time) > 400;
  


