drop table student;

create table student
	(
		id int unique,
		name varchar(32),
		score float,
		primary key(id)
	);

create table student
	(
		id int unique,
		name varchar(32),
		score float,
		primary key(id)
	);

create index name_index on student (name);
create index name_index on student (name);
drop index name_index;
drop index name_index;

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
insert into student
    values (7, "Taro", 80);

select score, name from student where id > 1 AND score < 110;
select * from student where id > 1 AND score < 110 AND name != "wahaha";
select * from student where name = "ZiRong Chen";
select * from student where score < 60;
select * from student where score > 50 AND name = "Haotian Xue";
select * from student;


delete from student where id >= 2 AND name = "ZiRong Chen";

delete from student where id >= 2 AND name = "ZiRong Chen" OR name = "Haotian Xue";

insert into student
	values (6, "John", 90);

select * from student;
