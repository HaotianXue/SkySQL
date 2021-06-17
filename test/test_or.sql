drop table student;

create table student
(
    id int unique,
    name varchar(32),
    score float,
    primary key(id)
);


insert into student
values (1, "Haotian Xue", 59.9);
insert into student
values (2, "ZiRong Chen", 59.8);
insert into student
values (3, "Tiga", 100);
insert into student
values (4, "HaHa", 120);
insert into student
values (5, "Haotian Xue", 90);


select * from student where score > 50 AND name = "Haotian Xue" OR score > 50 AND name = "ZiRong Chen" OR id >= 4;

select * from student where name = "ZiRong Chen" OR name = "Haotian Xue" AND score > 50;
select * from student where (name = "ZiRong Chen" OR name = "Haotian Xue") AND score > 60;

select * from student where (name = "ZiRong Chen" OR name = "Haotian Xue") OR (score > 60 AND name = "Tiga");
select * from student where (name = "ZiRong Chen" OR name = "Haotian Xue") OR score > 60 AND name = "Tiga";


select * from student where ((name = "ZiRong Chen" OR name = "Haotian Xue") OR score > 60) AND name = "Tiga";

select * from student where ((name = "ZiRong Chen" OR name = "Haotian Xue") OR score > 60) AND name = "Tiga" OR id >= 4;

select * from student where (((name = "ZiRong Chen" OR name = "Haotian Xue") OR score > 60) AND name = "Tiga") OR id >= 4;

delete from student where id = 2 AND name = "ZiRong Chen";

delete from student where id >= 2 AND name = "ZiRong Chen" OR name = "Haotian Xue";

select * from student;
