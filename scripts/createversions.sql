ALTER TABLE image DROP COLUMN `img_version`;
ALTER TABLE image ADD COLUMN `img_version` BIGINT UNSIGNED NOT NULL AFTER `img_sha1`;

DROP TABLE latestfiles;
CREATE TABLE latestfiles (
`version` BIGINT UNSIGNED NOT NULL,
`filename` CHAR(255) NOT NULL,
`mimetype` CHAR(32) NOT NULL,
PRIMARY KEY (`version`)
);

DROP TABLE filedata;
CREATE TABLE filedata (
`version` BIGINT UNSIGNED NOT NULL,
`data` LONGBLOB  NOT NULL,
PRIMARY KEY (`version`)
);

DROP PROCEDURE CREATEVERSIONS;

delimiter //
CREATE PROCEDURE CREATEVERSIONS ()
BEGIN
  DECLARE more INT DEFAULT TRUE;
  DECLARE sha1 VARBINARY(32);
  DECLARE stamp VARBINARY(14);
  DECLARE version, test_version BIGINT (20);
  DECLARE more_backup INT;
  DECLARE times CURSOR FOR SELECT img_timestamp, img_sha1, img_version FROM image;
  DECLARE test_times CURSOR FOR SELECT img_version FROM image WHERE img_version = version;
  DECLARE CONTINUE HANDLER FOR NOT FOUND SET more = FALSE;

  OPEN times;

  WHILE more DO
    FETCH times INTO stamp, sha1, version;
    SET more_backup = more;
    SET version = UNIX_TIMESTAMP(stamp) * 1000;
    OPEN test_times;
    FETCH test_times INTO test_version;
    WHILE more DO
      SET version = version + 1;
      FETCH test_times INTO test_version;
    END WHILE;
    CLOSE test_times;
    UPDATE image SET img_version=version WHERE img_timestamp=stamp AND img_sha1=sha1;
    SET more = more_backup;
  END WHILE;


  CLOSE times;
END//
delimiter ;
